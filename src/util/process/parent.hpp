
#ifndef UTIL_PROCESS_PARENT_HPP_
#define UTIL_PROCESS_PARENT_HPP_

#include <boost/function.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/asio/signal_set.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/smart_ptr/shared_ptr.hpp>

#include <list>

namespace util {

namespace process {

class child;
class child_options;

class parent
{
public:
	typedef boost::shared_ptr<child> child_pointer;
	typedef boost::function<void (child_pointer)> child_callback_t;

	parent (boost::asio::io_service& io_service);
	~parent ();

	bool start_child (const child_options& opts, const child_callback_t& cb);

	void stop_all ();

private:
	void queue_signal_handler ();
	void on_signal(int signal_number);

private:
	boost::asio::io_service& _io_service;
	boost::asio::signal_set _signal_set;
	boost::mutex _mutex;
	typedef std::list<std::pair<child_pointer, child_callback_t> > child_list_t;
	child_list_t _children;
};

}  // namespace process

}  // namespace util

#endif /* UTIL_PROCESS_PARENT_HPP_ */
