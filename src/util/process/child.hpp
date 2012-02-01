

#ifndef UTIL_PROCESS_CHILD_HPP_
#define UTIL_PROCESS_CHILD_HPP_

#include <boost/smart_ptr/shared_ptr.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition_variable.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/asio/posix/stream_descriptor.hpp>

#include "child_options.hpp"

#include <sys/types.h>

namespace util {

namespace process {

class parent;

class child
{
public:
	typedef boost::asio::posix::stream_descriptor stream_descriptor;
	typedef pid_t process_id;

	child (boost::asio::io_service& io_service,
			parent& p,
			const child_options& opts);
	~child ();

	void detach ();
	bool is_detached () const;

	bool is_running () const;
	int exit_code () const;
	void stop ();
	void wait ();

	process_id get_pid () const;

	const child_options& get_options () const;

	// throws std::invalid_argument if id does not exist
	stream_descriptor& get_stream (stream_id id);

private:
	friend class parent;

	boost::system::error_code start ();
	void report_stopped (int exit_code);

private:
	boost::asio::io_service& _io_service;
	parent& _parent;
	child_options _opts;
	mutable boost::mutex _mutex;
	boost::condition_variable _cond;
	process_id _pid;
	bool _detach;
	bool _running;
	bool _exited;
	int _exit_code;
	std::map<stream_id, boost::shared_ptr<stream_descriptor> > _streams;
};

}  // namespace process

}  // namespace util

#endif /* UTIL_PROCESS_CHILD_HPP_ */
