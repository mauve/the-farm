/*
 * Copyright (C) 2012, All rights reserved, Mikael Olenfalk <mikael@olenfalk.se>
 */

#include "message_queue.hpp"

#ifdef _WIN32
#	include <winsock2.h> // for ntohl and friends
#	undef max
#	undef min
#else
#	include <arpa/inet.h>
#endif

#include <stdexcept>

namespace network {

namespace transport {

struct message_queue::item
{
	enum { header, payload } state;
	message::header hdr;
	message::const_pointer msg;
	boost::uint32_t remains;

	item (message::const_pointer msg);

	boost::asio::const_buffer buffer () const;

	bool consume (std::size_t& num_bytes);
};

message_queue::item::item (message::const_pointer _msg)
	: state(header),
	  hdr(_msg->get_header()),
	  msg(_msg),
	  remains(sizeof(message::header))
{
	hdr.conversation_id = htonl(hdr.conversation_id);
	hdr.error_code = htonl(hdr.error_code);
	hdr.opcode = htonl(hdr.opcode);
	hdr.payload_length = htonl(hdr.payload_length);
}

bool message_queue::item::consume (std::size_t& num_bytes)
{
	if (num_bytes < remains) {
		remains -= num_bytes;
		num_bytes = 0;
		return false;
	} else {
		num_bytes -= remains;
		remains = 0;

		if (state == header) {
			state = payload;
			remains = msg->get_payload_length();
		}

		if (remains)
			return false;

		return true;
	}
}

message_queue::message_queue ()
{}

message_queue::~message_queue ()
{}

boost::asio::const_buffer message_queue::item::buffer () const
{
	using boost::asio::const_buffer;
	using std::size_t;

	const char* data = 0;
	size_t size = 0;
	if (state == header) {
		data = (const char*)&hdr;
		size = sizeof(message::header);
	} else {
		data = (const char*)msg->get_data();
		size = msg->get_payload_length();
	}

	return const_buffer(data + remains,
						size - remains);
}

bool message_queue::add (message::const_pointer msg)
{
	if (!msg)
		throw std::invalid_argument("msg cannot be null");

	bool was_empty = _queue.empty ();
	_queue.push_back (item(msg));
	return was_empty;
}

bool message_queue::empty () const
{
	return _queue.empty ();
}

void message_queue::consume (std::size_t& num_bytes)
{
	while (num_bytes && !_queue.empty ())
	{
		if (_queue.front ().consume (num_bytes))
			_queue.pop_front ();
	}
}

boost::asio::const_buffer message_queue::buffer () const
{
	if (_queue.empty ())
		throw std::invalid_argument ("queue is empty");

	return _queue.front ().buffer ();
}

}  // namespace transport

}  // namespace network


