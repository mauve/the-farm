/*
 * Copyright (C) 2012, All rights reserved, Mikael Olenfalk <mikael@olenfalk.se>
 */

#include <farm/compiler/runner.hpp>

#include <util/process/child.hpp>
#include <util/buffer/appendable_buffer.hpp>
#include <util/iostream/imemstream.hpp>
#include <util/iostream/readfile.hpp>

#include <farm/jobs/order.hpp>
#include <farm/jobs/result.hpp>
#include <farm/compiler/error_info.hpp>
#include <farm/compiler/error_parser.hpp>
#include <farm/compiler/description.hpp>
#include <farm/compiler/configured_runner_config.hpp>

#include <fstream>

#include <boost/bind.hpp>
#include <boost/filesystem.hpp>
#include <boost/smart_ptr/make_shared.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/nil_generator.hpp>
#include <boost/thread/mutex.hpp>

namespace farm {

using boost::shared_ptr;
using boost::array;
using boost::uint8_t;

namespace proc = util::process;
namespace fs = boost::filesystem;

struct CompilerRunner::running_child_t
{
	running_child_t ()
		: compilation_done(false),
		  stdout_closed(false),
		  stderr_closed(false),
		  child_exited(false)
	{}

	boost::mutex mutex;
	shared_ptr<jobs::JobOrder> order;
	shared_ptr<CompilerRunnerConfig> runner_config;
	fs::path jobdir;
	shared_ptr<jobs::JobResult> result;
	proc::parent::child_pointer child;
	job_callback_t callback;
	bool compilation_done;
	bool stdout_closed;
	bool stderr_closed;
	bool child_exited;

	util::buffer::appendable_buffer stdout_buf, stderr_buf;

	bool all_completed () const
	{
		return stdout_closed && stderr_closed && child_exited;
	}

	void prepare_child_start ()
	{
		stdout_closed = false;
		stderr_closed = false;
		child_exited = false;
		stdout_buf.reset();
		stderr_buf.reset();
	}
};

namespace {

void parse_errors (util::buffer::appendable_buffer& buf, ErrorParser& error_parser)
{
	util::imemstream im(buf.begin(), buf.end());
	std::string line;
	while (std::getline (im, line))
		error_parser.parse_line(line);
}

}  // namespace

CompilerRunner::CompilerRunner(boost::asio::io_service& io_service)
	: _io_service(io_service),
	  _parent(io_service)
{}

CompilerRunner::~CompilerRunner ()
{
	_parent.stop_all();
}

boost::system::error_code CompilerRunner::run_job(
		shared_ptr<jobs::JobOrder> order,
		shared_ptr<CompilerRunnerConfig> runner_config,
		const job_callback_t& cb)
{
	//
	// write input files
	//
	fs::path tmp = fs::temp_directory_path();
	tmp /= "job-" + boost::uuids::to_string(order->get_id());

	fs::create_directories(tmp);

	fs::path code_path(tmp / "code.cpp");
	{
		std::ofstream code_file(code_path.c_str());
		code_file.write(order->get_code().c_str(), order->get_code().length());
	}

	fs::path input_path(tmp / "input.file");
	if (!order->get_input().empty()) {
		std::ofstream input_file(input_path.c_str());
		input_file.write(order->get_input().c_str(), order->get_input().length());
	}

	fs::path output_path(tmp / "binary.exe");

	// build command line
	proc::child_options opts(runner_config->get_path());
	opts.set_args(runner_config->get_arguments(code_path.string(), output_path.string())).
		 set_stream_behaviour(proc::stream_id::standard_in, proc::child_options::close).
		 set_stream_behaviour(proc::stream_id::standard_out, proc::child_options::pipe_write).
		 set_stream_behaviour(proc::stream_id::standard_err, proc::child_options::pipe_write);
	// std::cerr << opts << std::endl;

	shared_ptr<running_child_t> child_data = boost::make_shared<running_child_t>();
	boost::unique_lock<boost::mutex> lock(child_data->mutex);
	child_data->order = order;
	child_data->runner_config = runner_config;
	child_data->jobdir = tmp;
	child_data->callback = cb;

	boost::system::error_code ec;
	child_data->child =
		_parent.start_child(opts,
							boost::bind(&CompilerRunner::_on_child_stopped, this,
											_1, child_data),
							ec);

	if (ec)
		return ec;

	_queue_read_stream (proc::stream_id::standard_out, child_data->stdout_buf, child_data);
	_queue_read_stream (proc::stream_id::standard_err, child_data->stderr_buf, child_data);

	return boost::system::error_code();
}

void CompilerRunner::_queue_read_stream(const proc::stream_id& sid,
											util::buffer::appendable_buffer& buf,
											shared_ptr<running_child_t>& child_data)
{
	child_data->child->get_stream(sid).
			async_read_some(boost::asio::buffer(buf.get_appendable_buffer(512)),
					boost::bind(&CompilerRunner::_on_read_stream, this, _1, _2,
									sid,
									boost::ref(buf),
									child_data));
}

void CompilerRunner::_on_read_stream(const boost::system::error_code& ec, std::size_t num_bytes_read,
										const util::process::stream_id& sid,
										util::buffer::appendable_buffer& buf,
										shared_ptr<running_child_t> child_data)
{
	boost::unique_lock<boost::mutex> lock(child_data->mutex);
	// std::cerr << "_on_read_stream: ec=" << ec << " sid=" << sid << std::endl;
	if (!ec) {
		// success
		buf.report_appended(num_bytes_read);
		_queue_read_stream(sid, buf, child_data);
	} else {
		if (sid == util::process::stream_id::standard_out)
			child_data->stdout_closed = true;
		else if (sid == util::process::stream_id::standard_err)
			child_data->stderr_closed = true;

		if (child_data->all_completed())
			_on_process_completed (child_data);
	}
}

void CompilerRunner::_on_child_stopped(proc::parent::child_pointer child, shared_ptr<running_child_t> child_data)
{
	boost::unique_lock<boost::mutex> lock(child_data->mutex);
	// std::cerr << "_on_child_stopped" << std::endl;
	child_data->child_exited = true;
	if (child_data->all_completed())
		_on_process_completed(child_data);
}

void CompilerRunner::_on_process_completed (shared_ptr<running_child_t>& child_data)
{
	// std::cerr << "_on_process_completed" << std::endl;
	running_child_t& cd = *child_data;

	if (!cd.compilation_done)
		cd.compilation_done = true;

	// actually supposed to start the process now, let's ignore that now
	ErrorParser& error_parser = cd.runner_config->get_error_parser();
	parse_errors(cd.stdout_buf, error_parser);
	parse_errors(cd.stderr_buf, error_parser);
	std::vector<ErrorInfo> errors = error_parser.parsed_errors();

	if (child_data->child->exit_code() != 0 && errors.empty()) {
		errors.push_back (ErrorInfo(ErrorInfo::error, "Compilation failed"));
	}

	//
	// create jobresult
	//

	// TODO: find the compiler which is being used
	CompilerDescription compiler_desc("gcc", "4.2-mac", Version(4, 2, 0, 0));

	cd.result.reset(new jobs::JobResult(boost::uuids::nil_uuid(),
											compiler_desc,
											errors));

	jobs::JobResult& result = *cd.result;
	cd.stdout_buf.move_to(result.output(jobs::JobResult::compiler_standard_output));
	cd.stderr_buf.move_to(result.output(jobs::JobResult::compiler_standard_error));

	fs::path binary_path(cd.jobdir / "binary.exe");
	util::readfile(binary_path.string(),
			result.output(jobs::JobResult::binary));

	child_data->callback(child_data->order, child_data->result);
}

}  // namespace farm
