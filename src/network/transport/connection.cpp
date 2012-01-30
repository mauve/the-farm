/*
 * Copyright (C) 2012, All rights reserved, Mikael Olenfalk <mikael@olenfalk.se>
 */

#include "connection.hpp"

#ifdef _WIN32
#	include <winsock2.h> // for ntohl and friends
#	undef max
#	undef min
#else
#	include <arpa/inet.h>
#endif

#include <algorithm>

namespace network {

namespace transport {

using boost::uint32_t;
using boost::unique_lock;
using boost::mutex;

connection::connection(boost::asio::io_service& io_service, uint32_t maximum_payload_length)
	: _io_service(io_service),
	  _socket(io_service),
	  _maximum_payload_length(maximum_payload_length),
	  _started(false),
	  _stopped(false)
{}

connection::pointer connection::create(boost::asio::io_service& io_service, uint32_t maximum_payload_length)
{
	return pointer(new connection(io_service, maximum_payload_length));
}

void connection::stop ()
{
	{
		unique_lock<mutex> lock(_mutex);
		_stopped = true;
	}

	boost::system::error_code ec;
	_socket.close (ec); // ignore error code
	_on_closed (shared_from_this());
}

void connection::start ()
{
	unique_lock<mutex> lock(_mutex);
	if (!_started)
		return;

	_queue_read_header ();
}

bool connection::running () const
{
	unique_lock<mutex> lock(_mutex);
	return _running();
}

bool connection::_running () const
{
	return _started && !_stopped;
}

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

void connection::send (message::const_pointer msg)
{
	unique_lock<mutex> lock(_mutex);
	if (!msg || !_running())
		return;

	if (_msg_queue.add(msg))
	{
		boost::asio::async_write(_socket,
			boost::asio::buffer(_msg_queue.buffer()),
			bind(&connection::_handle_write,
					shared_from_this(),
					boost::asio::placeholders::error(),
					boost::asio::placeholders::bytes_transferred()));
	}
}

boost::signals2::connection connection::connect_on_message(const on_message::slot_type& cb)
{
	return _on_message.connect(cb);
}

boost::signals2::connection connection::connect_on_closed(const on_closed::slot_type& cb)
{
	return _on_closed.connect(cb);
}

void connection::_handle_write(const boost::system::error_code& ec, std::size_t num_bytes)
{
	if (ec) {
		stop ();
		return;
	}

	unique_lock<mutex> lock(_mutex);
	_msg_queue.consume(num_bytes);

	if (!_running() || _msg_queue.empty())
		return;

	boost::asio::async_write(_socket,
		boost::asio::buffer(_msg_queue.buffer()),
		bind(&connection::_handle_write,
				shared_from_this(),
				boost::asio::placeholders::error(),
				boost::asio::placeholders::bytes_transferred()));
}

void connection::_queue_read_header()
{
	boost::shared_ptr<message::header> hdr(new message::header);

	boost::asio::async_read(_socket,
			boost::asio::buffer(hdr.get(), sizeof(message::header)),
			boost::bind(&connection::_handle_read_header,
							shared_from_this(),
							boost::asio::placeholders::error(),
							boost::asio::placeholders::bytes_transferred(),
							hdr));
}

void connection::_handle_read_header(const boost::system::error_code& ec,
										std::size_t num_bytes,
										boost::shared_ptr<message::header> hdr)
{
	if (ec) {
		stop ();
		return;
	}

	hdr->conversation_id = ntohl(hdr->conversation_id);
	hdr->error_code = ntohl(hdr->error_code);
	hdr->opcode = ntohl(hdr->opcode);
	hdr->payload_length = ntohl(hdr->payload_length);

	if (hdr->payload_length) {
		if (hdr->payload_length > get_maximum_payload_length())
			_queue_ignore_payload (hdr->payload_length);
		else
			_queue_read_payload(*hdr);
	}
	else
		_queue_read_header();
}

void connection::_queue_read_payload (const message::header& hdr)
{
	message::pointer msg = message::create(hdr);

	boost::asio::async_read(_socket,
			boost::asio::buffer(msg->get_data(), msg->get_payload_length()),
			boost::bind(&connection::_handle_read_payload,
							shared_from_this(),
							boost::asio::placeholders::error(),
							boost::asio::placeholders::bytes_transferred(),
							msg));
}

void connection::_handle_read_payload(const boost::system::error_code& ec, std::size_t num_bytes, message::pointer msg)
{
	if (ec) {
		stop ();
		return;
	}

	_on_message(shared_from_this(), msg);

	_queue_read_header();
}

void connection::_queue_ignore_payload (std::size_t num_bytes)
{
	ignore_buffer_t ignore_buffer(new ignore_buffer_t::value_type);

	boost::asio::async_read(_socket,
			boost::asio::buffer(
					ignore_buffer->data(),
					std::min(ignore_buffer->size(), num_bytes)),
			boost::bind(&connection::_handle_ignore_payload,
					shared_from_this(),
					boost::asio::placeholders::error(),
					boost::asio::placeholders::bytes_transferred(),
					ignore_buffer,
					num_bytes /* remains */));
}

void connection::_handle_ignore_payload(const boost::system::error_code& ec,
										std::size_t num_bytes,
										ignore_buffer_t ignore_buffer,
										std::size_t remains)
{
	if (ec) {
		stop ();
		return;
	}

	remains -= num_bytes;
	if (remains)
		_queue_ignore_payload(remains);
	else
		_queue_read_header();
}

}  // namespace transport

}  // namespace network
