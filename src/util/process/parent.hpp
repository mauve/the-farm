/*
 * Copyright (C) 2012, All rights reserved, Mikael Olenfalk <mikael@olenfalk.se>
 */

#ifndef UTIL_PROCESS_PARENT_HPP_
#define UTIL_PROCESS_PARENT_HPP_

#include <boost/function.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/asio/signal_set.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition_variable.hpp>
#include <boost/smart_ptr/shared_ptr.hpp>
#include <boost/system/error_code.hpp>

#include <list>

namespace util {

namespace process {

class child;
class child_options;

// TODO: inherit from enable_shared_from_this
class parent
{
public:
	typedef boost::shared_ptr<child> child_pointer;
	typedef boost::function<void (child_pointer)> child_callback_t;

	parent (boost::asio::io_service& io_service);
	~parent ();

	/* return true if child started successfully */
	child_pointer start_child (const child_options& opts,
								const child_callback_t& cb,
								boost::system::error_code& ec);

	void stop_all ();
	void join_all ();
	void detach_all ();

private:
	void queue_signal_handler ();
	void on_signal(const boost::system::error_code&, int signal_number);

private:
	boost::asio::io_service& _io_service;
	boost::asio::signal_set _signal_set;
	boost::mutex _mutex;
	boost::condition_variable _cond;
	typedef std::list<std::pair<child_pointer, child_callback_t> > child_list_t;
	child_list_t _children;
};

}  // namespace process

}  // namespace util

#endif /* UTIL_PROCESS_PARENT_HPP_ */
