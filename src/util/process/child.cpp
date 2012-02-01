
#include "child.hpp"

#include <boost/smart_ptr/scoped_array.hpp>
#include <boost/smart_ptr/shared_ptr.hpp>
#include <boost/smart_ptr/make_shared.hpp>
#include <boost/tuple/tuple.hpp>
#include <util/posix/pipe.hpp>

#include <signal.h>
#include <unistd.h>

namespace util {

namespace process {

namespace _detail {

namespace {

class zero_terminated_list_of_strings
{
public:
	zero_terminated_list_of_strings ()
	{}

	~zero_terminated_list_of_strings ()
	{
		for (std::vector<char*>::iterator iter = _buf.begin();
				iter != _buf.end(); ++iter)
		{
			delete[] *iter;
		}
	}

	void add (const std::string& s)
	{
		char* copy = new char[s.length() + 1];
		std::strcpy(copy, s.c_str());
		_buf.push_back(copy);
	}

	void add (const std::vector<std::string>& s)
	{
		for (std::vector<std::string>::const_iterator iter = s.begin();
				iter != s.end(); ++iter)
		{
			add(*iter);
		}
	}

	char** data()
	{
		if (_buf.empty() || _buf.back() != 0) {
			_buf.push_back(0);
		}

		return &*(_buf.begin());
	}

private:
	std::vector<char*> _buf;
};

}  // namespace anonymous

}  // namespace _detail

child::child (boost::asio::io_service& io_service,
				parent& p,
				const child_options& opts)
	: _io_service(io_service),
	  _parent(p),
	  _opts(opts),
	  _detach(false),
	  _running(false),
	  _exited(false)
{}

child::~child()
{
	if (!is_detached()) {
		stop ();
		wait ();
	}
}

void child::detach ()
{
	boost::unique_lock<boost::mutex> lock(_mutex);
	_detach = true;
}

bool child::is_detached () const
{
	boost::unique_lock<boost::mutex> lock(_mutex);
	return _detach;
}

bool child::is_running () const
{
	boost::unique_lock<boost::mutex> lock(_mutex);
	return _running;
}

int child::exit_code () const
{
	boost::unique_lock<boost::mutex> lock(_mutex);
	if (!_exited)
		return -1;
	return _exit_code;
}

void child::stop ()
{
	kill(_pid, SIGSTOP);
	// TODO: add timeout and SIGKILL and so on
}

void child::wait ()
{
	boost::unique_lock<boost::mutex> lock(_mutex);
	while (!_exited) {
		_cond.wait(lock);
	}
}

child::process_id child::get_pid () const
{
	boost::unique_lock<boost::mutex> lock(_mutex);
	return _pid;
}

const child_options& child::get_options () const
{
	return _opts;
}

child::stream_descriptor& child::get_stream (stream_id id)
{
	boost::unique_lock<boost::mutex> lock(_mutex);
	std::map<stream_id, boost::shared_ptr<stream_descriptor> >::iterator iter = _streams.find(id);
	if (iter == _streams.end())
		throw std::invalid_argument("no such stream id");

	return *(iter->second);
}

boost::system::error_code child::start ()
{
	using _detail::zero_terminated_list_of_strings;

	boost::unique_lock<boost::mutex> lock(_mutex);

	zero_terminated_list_of_strings argv;
	argv.add(_opts.get_command());
	argv.add(_opts.get_args());

	// TODO: handle env

	// setup pipes
	typedef std::vector<
				boost::tuple<
					stream_id,
					child_options::stream_behaviour,
					boost::shared_ptr<posix::pipe>
				>
			> pipes_t;
	pipes_t pipes;

	typedef std::map<stream_id, child_options::stream_behaviour> behav_map_t;
	const behav_map_t& stream_behaviours = _opts.get_stream_behaviours();
	for (behav_map_t::const_iterator iter = stream_behaviours.begin();
			iter != stream_behaviours.end(); ++iter)
	{
		if (iter->second == child_options::pipe_read ||
				iter->second == child_options::pipe_write)
		{
			pipes.push_back(
					boost::make_tuple(
						iter->first,
						iter->second,
						boost::make_shared<posix::pipe>()
					)
				);
		}
		else
		{
			pipes.push_back(
					boost::make_tuple(
						iter->first,
						iter->second,
						boost::shared_ptr<posix::pipe>()
					)
				);
		}
	}

	// unlock before fork
	lock.unlock();

	_io_service.notify_fork(boost::asio::io_service::fork_prepare);
	pid_t fork_result = fork ();
	if (fork_result < 0) {
		// error
		return boost::system::error_code(errno, boost::system::system_category());
	}
	else if (fork_result == 0) {
		//
		// we are the child
		//
		_io_service.notify_fork(boost::asio::io_service::fork_child);

		// dup and close the other end of the pipes
		for (pipes_t::iterator iter = pipes.begin();
				iter != pipes.end(); ++iter)
		{
			stream_id sid = boost::get<0>(*iter);
			child_options::stream_behaviour beh = boost::get<1>(*iter);
			boost::shared_ptr<posix::pipe> p = boost::get<2>(*iter);

			switch (beh)
			{
				case child_options::pipe_read: {
					::dup2(p->read_end(),
							sid.native());
					break;
				}

				case child_options::pipe_write: {
					::dup2(p->write_end(),
							sid.native());
					break;
				}

				case child_options::close: {
					::close(sid.native());
					break;
				}

				default:
					break;
			}

			p.reset();
		}

		// execute the child process
		::execv(_opts.get_command().c_str(),
				(char* const*)(argv.data()));
	}
	else if (fork_result > 0)
	{
		//
		// we are the parent
		//
		boost::unique_lock<boost::mutex> lock(_mutex);
		_io_service.notify_fork(boost::asio::io_service::fork_parent);
		_pid = fork_result;
		_running = true;
	}

	return boost::system::error_code();
}

void child::report_stopped(int exit_code)
{
	boost::unique_lock<boost::mutex> lock(_mutex);
	_running = false;
	_exited = true;
	_exit_code = exit_code;
	_cond.notify_all();
}

}  // namespace process

}  // namespace util
