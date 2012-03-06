/*
 * Copyright (C) 2012, All rights reserved, Mikael Olenfalk <mikael@olenfalk.se>
 */

#include "connection.hpp"

#include <algorithm>

namespace network {

namespace transport {

using boost::uint32_t;
using boost::unique_lock;
using boost::mutex;

connection::connection(boost::asio::io_service& io_service, uint32_t maximum_payload_length)
	: _io_service(io_service)
{}

uint32_t connection::get_maximum_payload_length() const
{
	unique_lock<mutex> lock(_mutex);
	return _maximum_payload_length;
}

uint32_t connection::set_maximum_payload_length(uint32_t mpl)
{
	unique_lock<mutex> lock(_mutex);
	uint32_t old = _maximum_payload_length;
	_maximum_payload_length = mpl;
	return old;
}

boost::signals2::connection connection::connect_on_message(const on_message::slot_type& cb)
{
	return _on_message.connect(cb);
}

boost::signals2::connection connection::connect_on_closed(const on_closed::slot_type& cb)
{
	return _on_closed.connect(cb);
}

}  // namespace transport

}  // namespace network
