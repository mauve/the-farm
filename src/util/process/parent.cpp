
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
	stop_all ();
}

bool parent::start_child (const child_options& opts, const child_callback_t& cb)
{
	boost::unique_lock<boost::mutex> lock(_mutex);
	child_pointer c(new child(_io_service, *this, opts));
	if (!c->start()) {
		// started successfully
		_children.push_back(std::make_pair(c, cb));
		return true;
	}

	return false;
}

void parent::stop_all ()
{
	boost::unique_lock<boost::mutex> lock(_mutex);
	child_list_t::iterator iter = _children.begin();
	for (; iter != _children.end(); ++iter)
		iter->first->stop();
}

void parent::queue_signal_handler()
{
	_signal_set.async_wait(
			boost::bind(&parent::on_signal, this, _2)
		);
}

void parent::on_signal(int signal_number)
{
	if (signal_number != SIGCHLD)
		return;

	boost::unique_lock<boost::mutex> lock(_mutex);

	int status = 0;
	pid_t pid;
    while ((pid = waitpid(-1, &status, WNOHANG)) > 0)
    {
    	child_list_t::iterator iter = _children.begin();
    	for (; iter != _children.end(); ++iter)
    	{
    		if (iter->first->get_pid() == pid) {
    			child_pointer child = iter->first;
    			_children.erase(iter);

    			if (WIFEXITED(status))
    				status = WEXITSTATUS(status);
    			else
    				status = -1; // TODO: export more status

    			child->report_stopped(status);

    			child_callback_t& cb = iter->second;

    			// call the callback a little later
    			// on the mainloop
    			_io_service.post(boost::bind(cb, child));
    		}
    	}
    }

    queue_signal_handler();
}



}  // namespace process

}  // namespace util
