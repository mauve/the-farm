/*
 * Copyright (C) 2012, All rights reserved, Mikael Olenfalk <mikael@olenfalk.se>
 */

#include "child.hpp"

#include <boost/smart_ptr/scoped_array.hpp>
#include <boost/smart_ptr/shared_ptr.hpp>
#include <boost/smart_ptr/make_shared.hpp>
#include <boost/tuple/tuple.hpp>

#include <signal.h>

#include <util/process/_detail/spawner.hpp>

#ifndef NDEBUG
#	include <iostream>
#	define DEBUG(x) std::cerr << x << std::endl
#else
#	define DEBUG(x)
#endif

namespace util {

namespace process {

child::child (boost::asio::io_service& io_service,
				parent& p,
				const child_options& opts)
	: _io_service(io_service),
	  _parent(p),
	  _opts(opts),
	  _detach(false),
	  _running(false),
	  _exited(false)
{
}

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

native_process_id child::get_pid () const
{
	boost::unique_lock<boost::mutex> lock(_mutex);
	return _pid;
}

const child_options& child::get_options () const
{
	return _opts;
}

stream_descriptor& child::get_stream (stream_id id)
{
	boost::unique_lock<boost::mutex> lock(_mutex);
	std::map<stream_id, boost::shared_ptr<stream_descriptor> >::iterator iter = _streams.find(id);
	if (iter == _streams.end())
		throw std::invalid_argument("no such stream id");

	return *(iter->second);
}

boost::system::error_code child::start ()
{
	boost::unique_lock<boost::mutex> lock(_mutex);

	boost::system::error_code ec;
	_pid = _detail::spawn(_io_service,
							_opts,
							_streams,
							ec);

	return ec;
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
