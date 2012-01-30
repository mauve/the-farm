/*
 * Copyright (C) 2012, All rights reserved, Mikael Olenfalk <mikael@olenfalk.se>
 */

#include "node_item.hpp"

namespace network {

namespace protocol {

using boost::uint16_t;

node_item::node_item (uint16_t type, uint16_t length)
	: _type(type), _length(length)
{}

uint16_t node_item::type () const
{
	return _type;
}

uint16_t node_item::length () const
{
	return _length;
}

void node_item::set_type(uint16_t type)
{
	_type = type;
}

void node_item::set_length(uint16_t length)
{
	_length = length;
}

}  // namespace protocol

}  // namespace network
