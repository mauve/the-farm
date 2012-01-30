/*
 * Copyright (C) 2012, All rights reserved, Mikael Olenfalk <mikael@olenfalk.se>
 */

#include <network/protocol/payload_stream.hpp>

#include <stdexcept>

namespace network {

namespace protocol {

payload_input_stream::payload_input_stream (const payload& p)
	: _begin(p.begin()), _iter(p.begin()), _end(p.end())
{}

payload_input_stream::~payload_input_stream ()
{}

bool payload_input_stream::empty () const
{
	return _iter == _end;
}

void payload_input_stream::reset ()
{
	_iter = _begin;
}

const payload_item_t& payload_input_stream::get ()
{
	if (_iter == _end)
		throw std::out_of_range ("payload_input_stream exhausted");

	return *_iter++;
}

/*
 *
 */

payload_output_stream::payload_output_stream (payload& p)
	: _payload(p)
{}

payload_output_stream::~payload_output_stream ()
{}
}  // namespace protocol

}  // namespace network


