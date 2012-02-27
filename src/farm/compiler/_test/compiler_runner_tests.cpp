/*
 * Copyright (C) 2012, All rights reserved, Mikael Olenfalk <mikael@olenfalk.se>
 */

#include <util/stupidtest/stupidtest.hpp>

#include <farm/jobs/order.hpp>
#include <farm/jobs/result.hpp>
#include <farm/compiler/runner.hpp>
#include <farm/compiler/configured_runner_config.hpp>
#include <farm/compiler/regex_error_parser.hpp>
#include <farm/compiler/description.hpp>

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/nil_generator.hpp>
#include <boost/thread.hpp>

using namespace farm;

using boost::shared_ptr;

const char* const source_code_valid =
	"#include <iostream>\n"
	"\n"
	"int main (int argc, char* argv[])\n"
	"{\n"
	"  std::cout << \"Hello\" << std::endl;\n"
	"  return 0;\n"
	"}\n"
	"\n";

const char* const source_code_invalid =
	"#include <iostream>\n"
	"\n"
	"int main (int argc, char* argv[])\n"
	"{\n"
	"  std::cout << \"Hello\" << std::endl;\n"
	"  apa ();\n" // ERROR: missing decl apa
	"}\n"
	"\n";


struct runner_test_fixture
{
	std::vector<RegexErrorParser::ParseSetting> patterns;
	boost::shared_ptr<RegexErrorParser> error_parser;
	boost::shared_ptr<ConfiguredCompilerRunnerConfig> runner_config;
	boost::shared_ptr<jobs::JobOrder> job_order;

	runner_test_fixture (
				const char* const gcc_path,
				const char* const source_code,
				const char* const input
			)
	{
		typedef RegexErrorParser::ParseSetting PS;
		patterns.push_back(PS(ErrorInfo::error,
				"(.*?):(\\d+):(\\d+)? [Ee]rror: (.*)", 1, 2, 3, 4));

		error_parser.reset(new RegexErrorParser(
					patterns,
					std::vector<std::string>()
				));

		std::vector<std::string> args;
		args.push_back("-o");
		args.push_back("${target-path}");
		args.push_back("${code-path}");
		runner_config.reset(new ConfiguredCompilerRunnerConfig(
					gcc_path,
					args,
					error_parser
				));

		job_order.reset(new jobs::JobOrder(
					boost::uuids::nil_uuid(),
					std::vector<CompilerDescription>(),
					source_code,
					input
				));
	}
};

struct job_callback
{
	job_callback ()
		: _has_been_called(false)
	{}

	void operator() (shared_ptr<jobs::JobOrder> order, shared_ptr<jobs::JobResult> result)
	{
		boost::unique_lock<boost::mutex> lock(_mutex);
		_has_been_called = true;
		_order = order;
		_result = result;
		_cond.notify_one();
	}

	void wait ()
	{
		boost::unique_lock<boost::mutex> lock(_mutex);
		while (!_has_been_called)
			_cond.wait (lock);
	}

	bool has_been_called ()
	{
		boost::unique_lock<boost::mutex> lock(_mutex);
		return _has_been_called;
	}

	boost::shared_ptr<jobs::JobOrder> get_order ()
	{
		boost::unique_lock<boost::mutex> lock(_mutex);
		return _order;
	}

	boost::shared_ptr<jobs::JobResult> get_result ()
	{
		boost::unique_lock<boost::mutex> lock(_mutex);
		return _result;
	}

private:
	boost::mutex _mutex;
	boost::condition_variable _cond;
	bool _has_been_called;
	boost::shared_ptr<jobs::JobOrder> _order;
	boost::shared_ptr<jobs::JobResult> _result;
};

void test_run_compiler (boost::asio::io_service& io_service)
{
	runner_test_fixture fixture("/usr/bin/g++", source_code_valid, "");
	CompilerRunner runner(io_service);

	job_callback callback;
	runner.run_job(fixture.job_order, fixture.runner_config,
			boost::ref(callback));

	callback.wait();
	boost::shared_ptr<jobs::JobResult> res =
			callback.get_result();

	ST_CHECK(!!res);
	ST_CHECK(res->succeeded());
	ST_CHECK(res->messages().empty());
}

void test_run_compiler2 (boost::asio::io_service& io_service)
{
	runner_test_fixture fixture("/usr/bin/g++", source_code_invalid, "");
	CompilerRunner runner(io_service);

	job_callback callback;
	runner.run_job(fixture.job_order, fixture.runner_config,
			boost::ref(callback));

	callback.wait();
	boost::shared_ptr<jobs::JobResult> res =
			callback.get_result();

	ST_CHECK(!!res);
	ST_CHECK(!res->succeeded());
	ST_CHECK(!res->messages().empty());

	const std::vector<ErrorInfo>& errors = res->messages();
	for (std::vector<ErrorInfo>::const_iterator iter = errors.begin();
			iter != errors.end(); ++iter)
	{
		std::cout << "[message found] " << *iter << std::endl;
	}
}

void thread_runner (boost::asio::io_service& io_service)
{
	io_service.run ();
}

int main (int argc, char* argv[])
{
	util::stupidtest::setup("compiler_runner_tests", false);

	boost::asio::io_service io_service;
	boost::asio::io_service::work* work = new boost::asio::io_service::work(io_service);
	boost::thread io_thread(thread_runner, boost::ref(io_service));

	try {
		ST_RUN_TEST(test_run_compiler(io_service));
		ST_RUN_TEST(test_run_compiler2(io_service));
	} catch (...) {
		delete work;
		io_thread.join ();
		return 1;
	}

	delete work;
	io_thread.join ();
	return 0;
}
