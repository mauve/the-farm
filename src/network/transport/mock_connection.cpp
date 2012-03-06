/*
 * Copyright (C) 2012, All rights reserved, Mikael Olenfalk <mikael@olenfalk.se>
 */

#include "mock_connection.hpp"

#include <algorithm>

namespace network {

namespace transport {

using boost::uint32_t;
using boost::unique_lock;
using boost::mutex;

mock_connection::mock_connection(boost::asio::io_service& io_service, uint32_t maximum_payload_length)
	: connection(io_service, maximum_payload_length),
	  _socket(io_service),
	  _started(false),
	  _stopped(false)
{}

mock_connection::pointer mock_connection::create(boost::asio::io_service& io_service, uint32_t maximum_payload_length)
{
	return pointer(new mock_connection(io_service, maximum_payload_length));
}

void mock_connection::stop ()
{
	{
		unique_lock<mutex> lock(_mutex);
		_stopped = true;
	}

	_on_closed ();
}

void mock_connection::start ()
{
	unique_lock<mutex> lock(_mutex);
	if (_started)
		return;

	_started = true;
}

bool mock_connection::running () const
{
	unique_lock<mutex> lock(_mutex);
	return _running();
}

bool mock_connection::_running () const
{
	return _started && !_stopped;
}

void mock_connection::send (message::const_pointer msg)
{
	unique_lock<mutex> lock(_mutex);
	if (!msg || !_running())
		return;

	_messages.push_back(msg);
}

std::deque<message::const_pointer>& mock_connection::sent_messages ()
{
	return _messages;
}

void mock_connection::simulate_message_received(message::const_pointer msg)
{
	_on_message(msg);
}

}  // namespace transport

}  // namespace network
