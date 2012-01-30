/*
 * Copyright (C) 2012, All rights reserved, Mikael Olenfalk <mikael@olenfalk.se>
 */

#include "message.hpp"

namespace network {

namespace transport {

using boost::uint32_t;

message::header::header ()
	: conversation_id(0),
	  opcode(0),
	  error_code(0),
	  payload_length(0)
{}

message::header::header (uint32_t cid, uint32_t op,
							uint32_t mt, uint32_t ec)
	: conversation_id(cid),
	  opcode(op),
	  message_type(mt),
	  error_code(ec),
	  payload_length(0)
{}

message::pointer message::create (const header& hdr)
{
	return pointer(new message(hdr));
}

message::pointer message::create (const header& hdr, uint32_t payload_length)
{
	return pointer(new message(hdr, payload_length));
}

message::pointer message::create (const header& hdr, const std::vector<boost::uint8_t>& payload)
{
	return pointer(new message(hdr, payload));
}

message::message (const header& hdr)
	: _header(hdr), _payload(hdr.payload_length)
{}

message::message (const header& hdr, boost::uint32_t payload_length)
	: _header(hdr), _payload(payload_length)
{
	_header.payload_length = payload_length;
}

message::message (const header& hdr, const std::vector<boost::uint8_t>& payload)
	: _header(hdr), _payload(payload)
{
	_header.payload_length = _payload.size();
}

message::~message ()
{}

const message::header& message::get_header () const
{
	return _header;
}

void message::set_header (const header& hdr)
{
	_header = hdr;
	_payload.resize(_header.payload_length, 0);
}

uint32_t message::get_payload_length () const
{
	return _payload.size ();
}

const boost::uint8_t* message::get_data () const
{
	return 0;
}

boost::uint8_t* message::get_data ()
{
	return 0;
}

}  // namespace transport

}  // namespace network


