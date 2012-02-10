
/*
 * cannot use Boost.Test because it swallows SIGCHLD
 */

#include <iostream>

#include <util/process/parent.hpp>
#include <util/process/child.hpp>
#include <util/process/child_options.hpp>
#include <boost/thread.hpp>

using util::process::parent;
using util::process::child;
using util::process::child_options;

struct check_exception
{};

bool verbose = false;

#define CHECK(x) \
	if (verbose) { \
		std::cerr << "   Check: " << #x << ": " << std::flush; \
	} \
	if (!(x)) { \
		if (!verbose) \
			std::cerr << "Check failed: " << #x << std::endl; \
		else \
			std::cerr << "failed." << std::endl; \
		throw check_exception(); \
	} else if (verbose) { \
		std::cerr << "succeeded." << std::endl; \
	}

#define RUN_TEST(x) \
	try { \
		std::cerr << "Running test: " << #x << std::endl; \
		x; \
		std::cerr << "   ...succeeded" << std::endl; \
		\
	} catch (const check_exception&) { \
		std::cerr << "   ...failed" << std::endl; \
		return 1; \
	} catch (const std::exception& ex) { \
		std::cerr << "   ...exception: " << ex.what() << std::endl; \
		return 1; \
	}



struct child_callback
{
public:
	child_callback ()
		: _has_been_called(false)
	{}

	void operator() (parent::child_pointer c)
	{
		boost::unique_lock<boost::mutex> lock(_mutex);
		_has_been_called = true;
		_child = c;
		_cond.notify_all();
	}

	void wait ()
	{
		boost::unique_lock<boost::mutex> lock(_mutex);
		while (!_has_been_called)
			_cond.wait(lock);
	}

	bool has_been_called()
	{
		boost::unique_lock<boost::mutex> lock(_mutex);
		return _has_been_called;
	}

	parent::child_pointer get_child()
	{
		boost::unique_lock<boost::mutex> lock(_mutex);
		return _child;
	}

private:
	boost::mutex _mutex;
	boost::condition_variable _cond;
	bool _has_been_called;
	parent::child_pointer _child;
};

struct read_callback
{
public:
	typedef void result_type;

	enum { data_size = 4096 };

	read_callback ()
		: _has_been_called(false), _bytes_read(0)
	{}

	~read_callback ()
	{}

	void operator() (const boost::system::error_code& ec, std::size_t bytes_read)
	{
		boost::unique_lock<boost::mutex> lock(_mutex);
		_has_been_called = true;
		_bytes_read = bytes_read;
		_cond.notify_all();
	}

	bool has_been_called()
	{
		boost::unique_lock<boost::mutex> lock(_mutex);
		return _has_been_called;
	}

	std::size_t bytes_read()
	{
		boost::unique_lock<boost::mutex> lock(_mutex);
		return _bytes_read;
	}

	std::string data_read()
	{
		boost::unique_lock<boost::mutex> lock(_mutex);
		return std::string(_data, _data + _bytes_read);
	}

	char* raw_data()
	{
		return _data;
	}

	void wait ()
	{
		boost::unique_lock<boost::mutex> lock(_mutex);
		while (!_has_been_called)
			_cond.wait(lock);
	}

private:
	boost::mutex _mutex;
	boost::condition_variable _cond;
	bool _has_been_called;
	std::size_t _bytes_read;
	char _data[data_size];
};

struct write_callback
{
public:
	typedef void result_type;

	write_callback (const std::string& data)
		: _data(data), _has_been_called(false), _bytes_written(0)
	{}

	~write_callback ()
	{}

	void operator() (const boost::system::error_code& ec, std::size_t bytes_written)
	{
		boost::unique_lock<boost::mutex> lock(_mutex);
		_has_been_called = true;
		_bytes_written = bytes_written;
		_cond.notify_all();
	}

	bool has_been_called()
	{
		boost::unique_lock<boost::mutex> lock(_mutex);
		return _has_been_called;
	}

	std::size_t bytes_written()
	{
		boost::unique_lock<boost::mutex> lock(_mutex);
		return _bytes_written;
	}

	const char* raw_data()
	{
		return _data.c_str();
	}

	std::size_t data_size()
	{
		return _data.length();
	}

	void wait ()
	{
		boost::unique_lock<boost::mutex> lock(_mutex);
		while (!_has_been_called)
			_cond.wait(lock);
	}

private:
	boost::mutex _mutex;
	boost::condition_variable _cond;
	std::string _data;
	bool _has_been_called;
	std::size_t _bytes_written;
};

void thread_runner (boost::asio::io_service& io_service)
{
	io_service.run ();
}

void test_start_child_process (boost::asio::io_service& io_service)
{
	parent p(io_service);

	child_callback callback;
	parent::child_pointer child = p.start_child(child_options(CHILD_PROCESS_PATH), boost::ref(callback));
	CHECK(!!child);
	CHECK(child->get_pid());

	callback.wait();
	CHECK(callback.has_been_called());

	child = callback.get_child();
	CHECK(!!child);
	CHECK(!child->is_running());
	CHECK(child->exit_code() == 0);

	return;
}

void test_start_child_process_ignore_stderr_and_stdout (boost::asio::io_service& io_service)
{
	parent p(io_service);

	child_callback callback;
	child_options opts(CHILD_PROCESS_PATH);
	opts.set_stream_behaviour(util::process::stream_id::standard_out, child_options::close);
	opts.set_stream_behaviour(util::process::stream_id::standard_err, child_options::close);
	parent::child_pointer child = p.start_child(opts, boost::ref(callback));
	CHECK(!!child);
	CHECK(child->get_pid());

	callback.wait();
	CHECK(callback.has_been_called());

	child = callback.get_child();
	CHECK(!!child);
	CHECK(!child->is_running());
	CHECK(child->exit_code() == 0);

	return;
}

void test_start_child_process_return_code (boost::asio::io_service& io_service)
{
	parent p(io_service);

	child_callback callback;
	parent::child_pointer child = p.start_child(child_options(CHILD_PROCESS_PATH).add_arg("2"), boost::ref(callback));
	CHECK(!!child);
	CHECK(child->get_pid());

	callback.wait();
	CHECK(callback.has_been_called());

	child = callback.get_child();
	CHECK(!!child);
	CHECK(!child->is_running());
	CHECK(child->exit_code() == 1);

	return;
}

void test_start_child_process_read_stdout (boost::asio::io_service& io_service)
{
	parent p(io_service);

	child_callback callback;
	child_options opts(CHILD_PROCESS_PATH);
	opts.set_stream_behaviour(util::process::stream_id::standard_out, child_options::pipe_write);
	parent::child_pointer child = p.start_child(opts, boost::ref(callback));
	CHECK(!!child);
	CHECK(child->get_pid());

	child::stream_descriptor& standard_out = child->get_stream(util::process::stream_id::standard_out);
	read_callback read_cb;
	standard_out.async_read_some(boost::asio::buffer(read_cb.raw_data(), read_callback::data_size),
									boost::bind(boost::ref(read_cb), _1, _2));

	read_cb.wait();
	CHECK(read_cb.has_been_called());
	CHECK(read_cb.data_read() == "Hello World!\n");

	callback.wait();
	CHECK(callback.has_been_called());

	child = callback.get_child();
	CHECK(!!child);
	CHECK(!child->is_running());
	CHECK(child->exit_code() == 0);

	return;
}

void test_start_child_process_read_stdout_and_stderr (boost::asio::io_service& io_service)
{
	parent p(io_service);

	child_callback callback;
	child_options opts(CHILD_PROCESS_PATH);
	opts.set_stream_behaviour(util::process::stream_id::standard_out, child_options::pipe_write);
	opts.set_stream_behaviour(util::process::stream_id::standard_err, child_options::pipe_write);
	opts.add_arg("4");

	parent::child_pointer child = p.start_child(opts, boost::ref(callback));
	CHECK(!!child);
	CHECK(child->get_pid());

	child::stream_descriptor& standard_out = child->get_stream(util::process::stream_id::standard_out);
	read_callback read_stdout;
	standard_out.async_read_some(boost::asio::buffer(read_stdout.raw_data(), read_callback::data_size),
									boost::bind(boost::ref(read_stdout), _1, _2));

	child::stream_descriptor& standard_err = child->get_stream(util::process::stream_id::standard_err);
	read_callback read_stderr;
	standard_err.async_read_some(boost::asio::buffer(read_stderr.raw_data(), read_callback::data_size),
									boost::bind(boost::ref(read_stderr), _1, _2));

	read_stdout.wait();
	CHECK(read_stdout.has_been_called());
	CHECK(read_stdout.data_read() == "Hello Out World!\n");

	read_stderr.wait();
	CHECK(read_stderr.has_been_called());
	CHECK(read_stderr.data_read() == "Hello Err World!\n");

	callback.wait();
	CHECK(callback.has_been_called());

	child = callback.get_child();
	CHECK(!!child);
	CHECK(!child->is_running());
	CHECK(child->exit_code() == 0);

	return;
}

void test_start_child_process_write_stdin_and_read_stdout_and_stderr (boost::asio::io_service& io_service)
{
	parent p(io_service);

	child_callback callback;
	child_options opts(CHILD_PROCESS_PATH);
	opts.set_stream_behaviour(util::process::stream_id::standard_in, child_options::pipe_read);
	opts.set_stream_behaviour(util::process::stream_id::standard_out, child_options::pipe_write);
	opts.set_stream_behaviour(util::process::stream_id::standard_err, child_options::pipe_write);
	opts.add_arg("3");

	parent::child_pointer child = p.start_child(opts, boost::ref(callback));
	CHECK(!!child);
	CHECK(child->get_pid());

	child::stream_descriptor& standard_in = child->get_stream(util::process::stream_id::standard_in);
	write_callback write_stdin("Smurf\n");
	standard_in.async_write_some(boost::asio::buffer(write_stdin.raw_data(), write_stdin.data_size()),
									boost::bind(boost::ref(write_stdin), _1, _2));

	child::stream_descriptor& standard_out = child->get_stream(util::process::stream_id::standard_out);
	read_callback read_stdout;
	standard_out.async_read_some(boost::asio::buffer(read_stdout.raw_data(), read_callback::data_size),
									boost::bind(boost::ref(read_stdout), _1, _2));

	child::stream_descriptor& standard_err = child->get_stream(util::process::stream_id::standard_err);
	read_callback read_stderr;
	standard_err.async_read_some(boost::asio::buffer(read_stderr.raw_data(), read_callback::data_size),
									boost::bind(boost::ref(read_stderr), _1, _2));

	write_stdin.wait();
	CHECK(write_stdin.has_been_called());
	CHECK(write_stdin.bytes_written() == 6);

	read_stdout.wait();
	CHECK(read_stdout.has_been_called());
	CHECK(read_stdout.data_read() == "Smurf\n");

	read_stderr.wait();
	CHECK(read_stderr.has_been_called());
	CHECK(read_stderr.data_read() == "Smurf\n");

	callback.wait();
	CHECK(callback.has_been_called());

	child = callback.get_child();
	CHECK(!!child);
	CHECK(!child->is_running());
	CHECK(child->exit_code() == 0);

	return;
}

int main (int argc, char* argv[])
{
	verbose = true;
	std::cout << "CHILD_PROCESS_PATH=" << CHILD_PROCESS_PATH << std::endl;

	boost::asio::io_service io_service;
	boost::asio::io_service::work* work = new boost::asio::io_service::work(io_service);
	boost::thread io_thread(thread_runner, boost::ref(io_service));

	try {
		RUN_TEST(test_start_child_process(io_service));
		RUN_TEST(test_start_child_process_ignore_stderr_and_stdout(io_service));
		RUN_TEST(test_start_child_process_return_code(io_service));
		RUN_TEST(test_start_child_process_read_stdout(io_service));
		RUN_TEST(test_start_child_process_read_stdout_and_stderr(io_service));
		RUN_TEST(test_start_child_process_write_stdin_and_read_stdout_and_stderr(io_service));
	} catch (...) {
		delete work;
		io_thread.join ();
		return 1;
	}

	return 0;
}
