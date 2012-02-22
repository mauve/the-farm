
#include <farm/compiler/runner.hpp>

#include <util/process/child.hpp>
#include <util/buffer/appendable_buffer.hpp>
#include <util/iostream/imemstream.hpp>

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

namespace farm {

using boost::shared_ptr;
using boost::array;
using boost::uint8_t;

namespace proc = util::process;
namespace fs = boost::filesystem;

struct CompilerRunner::running_child_t
{
	running_child_t ()
		: compilation_done(false)
	{}

	shared_ptr<jobs::JobOrder> order;
	shared_ptr<CompilerRunnerConfig> runner_config;
	fs::path jobdir;
	shared_ptr<jobs::JobResult> result;
	proc::parent::child_pointer child;
	job_callback_t callback;
	bool compilation_done;

	util::buffer::appendable_buffer stdout_buf, stderr_buf;
};

CompilerRunner::CompilerRunner(boost::asio::io_service& io_service)
	: _io_service(io_service),
	  _parent(io_service)
{}

CompilerRunner::~CompilerRunner ()
{
	_parent.stop_all();
}

void CompilerRunner::run_job(shared_ptr<jobs::JobOrder> order,
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

	shared_ptr<running_child_t> child_data = boost::make_shared<running_child_t>();
	child_data->order = order;
	child_data->runner_config = runner_config;
	child_data->jobdir = tmp;
	child_data->callback = cb;
	child_data->child =
			_parent.start_child(opts, boost::bind(&CompilerRunner::_on_child_stopped, this,
													_1, child_data));

	_queue_read_stream (proc::stream_id::standard_out, child_data->stdout_buf, child_data);
	_queue_read_stream (proc::stream_id::standard_err, child_data->stderr_buf, child_data);
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
	if (!ec) {
		// success
		buf.report_appended(num_bytes_read);
		_queue_read_stream(sid, buf, child_data);
	}
}

void CompilerRunner::_on_child_stopped(proc::parent::child_pointer child, shared_ptr<running_child_t> child_data)
{
	running_child_t& cd = *child_data;

	if (!cd.compilation_done)
		cd.compilation_done = true;

	// actually supposed to start the process now, let's ignore that now
	// TODO: find the compiler which is being used
	CompilerDescription compiler_desc("gcc", "4.2-mac", Version(4, 2, 0, 0));

	std::vector<ErrorInfo> errors;
	{
		std::string line;
		util::imemstream ims(cd.stdout_buf.begin(), cd.stdout_buf.end());
		while (std::getline(ims, line))
			cd.runner_config->get_error_parser().parse_line(line);
		errors = cd.runner_config->get_error_parser().parsed_errors();
	}

	{
		std::string line;
		util::imemstream ims(cd.stderr_buf.begin(), cd.stderr_buf.end());
		while (std::getline(ims, line))
			cd.runner_config->get_error_parser().parse_line(line);
		errors.insert(errors.end(),
						cd.runner_config->get_error_parser().parsed_errors().begin(),
						cd.runner_config->get_error_parser().parsed_errors().end());
	}

	if (child->exit_code() != 0 && errors.empty()) {
		errors.push_back (ErrorInfo(ErrorInfo::error, "Compilation failed"));
	}

	cd.result.reset(new jobs::JobResult(boost::uuids::nil_uuid(),
											compiler_desc,
											errors));

	jobs::JobResult& result = *cd.result;
	cd.stdout_buf.move_to(result.output(jobs::JobResult::compiler_standard_output));
	cd.stderr_buf.move_to(result.output(jobs::JobResult::compiler_standard_error));

	{
		fs::path binary_path(cd.jobdir / "binary.exe");
		std::ifstream file(binary_path.c_str());
		if (file.is_open()) {
			file.seekg(0, std::ifstream::end);
			std::size_t size = file.tellg();
			file.seekg(0, std::ifstream::beg);

			if (size) { // TODO: add upper limit
				std::vector<char>& buf = result.output(jobs::JobResult::binary);
				buf.resize(size);
				file.read(&buf[0], size);
			}
		}
	}

	child_data->callback(child_data->order, child_data->result);
}

}  // namespace farm
