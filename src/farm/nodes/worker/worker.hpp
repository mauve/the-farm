/*
 * Copyright (C) 2012, All rights reserved, Mikael Olenfalk <mikael@olenfalk.se>
 */

#ifndef FARM_NODES_WORKER_HPP_
#define FARM_NODES_WORKER_HPP_

#include <boost/asio/io_service.hpp>

#include <network/protocol/dispatcher.hpp>
#include <network/transport/message_passer.hpp>

namespace farm {

namespace nodes {

namespace worker {

class Worker
{
public:
	Worker (boost::asio::io_service& io_service,
			network::transport::message_passer::pointer mpi);
	~Worker ();

	void start ();
	void stop ();

private:
	void _on_queue_job (const jobs::JobOrder& order);
	void _on_query_job_queues ();
	void _on_cancel_job ();

private:
	boost::asio::io_service& _io_service;
	network::transport::message_passer::pointer _mpi;
	network::protocol::dispatcher<Worker> _dispatcher;
};

}  // namespace worker

}  // namespace nodes

}  // namespace farm

#endif /* FARM_NODES_WORKER_HPP_ */
