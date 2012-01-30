/*
 * Copyright (C) 2012, All rights reserved, Mikael Olenfalk <mikael@olenfalk.se>
 */

#include "node.hpp"

namespace farm {

Node::Node (boost::asio::io_service& io_service,
			const boost::uuids::uuid& node_id)
	: _io_service(io_service), _node_id(node_id),
	  _started(false), _stopped(false)
{}

Node::~Node ()
{}

const boost::uuids::uuid& Node::get_id () const
{
	return _node_id;
}

void Node::start ()
{
	boost::unique_lock<boost::mutex> lock(_mutex);
	if (_started || _stopped)
		return;

	_started = true;
}

void Node::stop ()
{
	boost::unique_lock<boost::mutex> lock(_mutex);
	if (_started || _stopped)
		return;

	_started = false;
	_stopped = true;
}

}  // namespace farm



