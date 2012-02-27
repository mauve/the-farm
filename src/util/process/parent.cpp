/*
 * Copyright (C) 2012, All rights reserved, Mikael Olenfalk <mikael@olenfalk.se>
 */

#include "parent.hpp"
#include "child.hpp"

#include <boost/bind.hpp>

#include <signal.h>
#include <sys/wait.h>

namespace util {

namespace process {

parent::parent (boost::asio::io_service& io_service)
	: _io_service(io_service),
	  _signal_set(io_service, SIGCHLD)
{
	queue_signal_handler ();
}

parent::~parent()
{
	detach_all();
}

parent::child_pointer parent::start_child (const child_options& opts,
											const child_callback_t& cb,
											boost::system::error_code& ec)
{
	boost::unique_lock<boost::mutex> lock(_mutex);
	child_pointer c(new child(_io_service, *this, opts));

	ec = c->start ();
	if (!ec) {
		// started successfully
		_children.push_back(std::make_pair(c, cb));
		return c;
	}

	return child_pointer();
}

void parent::stop_all ()
{
	boost::unique_lock<boost::mutex> lock(_mutex);
	child_list_t::iterator iter = _children.begin();
	for (; iter != _children.end(); ++iter)
		iter->first->stop();
}

void parent::join_all ()
{
	boost::unique_lock<boost::mutex> lock(_mutex);
	while (!_children.empty())
		_cond.wait(lock);
}

void parent::detach_all ()
{
	boost::unique_lock<boost::mutex> lock(_mutex);
	for (child_list_t::iterator iter = _children.begin();
			iter != _children.end();)
	{
		iter->first->detach();
		iter = _children.erase(iter);
	}
}

void parent::queue_signal_handler()
{
	// std::cerr << "queue_signal_handler" << std::endl;
	_signal_set.async_wait(
			boost::bind(&parent::on_signal, this, _1, _2)
		);
}

void parent::on_signal(const boost::system::error_code& ec, int signal_number)
{
	if (ec || signal_number != SIGCHLD)
		return;

	boost::unique_lock<boost::mutex> lock(_mutex);

	int status = 0;
	pid_t pid;
    while ((pid = waitpid(-1, &status, WNOHANG)) > 0)
    {
    	// std::cerr << "on_signal: ec=" << ec << " pid=" << pid << std::endl;
    	child_list_t::iterator iter = _children.begin();
    	for (; iter != _children.end(); ++iter)
    	{
    		if (iter->first->get_pid() == pid) {
    			child_pointer child = iter->first;

    			if (WIFEXITED(status))
    				status = WEXITSTATUS(status);
    			else
    				status = -1; // TODO: export more status

    			child->report_stopped(status);

    			child_callback_t& cb = iter->second;
    			// std::cerr << "on_signal: calling callback..." << std::endl;
    			cb(child);
    			// std::cerr << "on_signal: ...callback done" << std::endl;

    			_children.erase(iter);
    		}
    	}

    	_cond.notify_all();
    }

    queue_signal_handler();
}



}  // namespace process

}  // namespace util
