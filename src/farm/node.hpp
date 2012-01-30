/*
 * Copyright (C) 2012, All rights reserved, Mikael Olenfalk <mikael@olenfalk.se>
 */

#ifndef __FARM_NODE_HPP__
#define __FARM_NODE_HPP__

#include <boost/function.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/thread/mutex.hpp>

#include <deque>

namespace farm {

class Node
{
public:
	enum JobState { queued, running, failed, finished };
	typedef boost::function<void (JobState)> job_callback_t;

	Node (boost::asio::io_service& io_service,
			const boost::uuids::uuid& node_id);
	~Node ();

	const boost::uuids::uuid& get_id () const;

	void start ();
	void stop ();

	void queue_job (boost::shared_ptr<JobOrder> order, const job_callback_t& cb);

private:
	boost::asio::io_service& _io_service;
	boost::uuids::uuid _node_id;
	boost::mutex _mutex;
	bool _started, _stopped;
	std::deque<boost::shared_ptr<JobOrder> > _orders;
};

}  // namespace farm


#endif /* __FARM_NODE_HPP__ */
