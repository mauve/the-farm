/*
 * Copyright (C) 2012, All rights reserved, Mikael Olenfalk <mikael@olenfalk.se>
 */

#ifndef NETWORK_PROTOCOL_NODE_ITEM_HPP_
#define NETWORK_PROTOCOL_NODE_ITEM_HPP_

#include <boost/cstdint.hpp>

namespace network {

namespace protocol {

class node_item
{
public:
	node_item (boost::uint16_t type, boost::uint16_t length);

	boost::uint16_t type () const;
	boost::uint16_t length () const;

	void set_type (boost::uint16_t type);
	void set_length (boost::uint16_t length);

private:
	boost::uint16_t _type, _length;
};

}  // namespace protocol

}  // namespace network


#endif /* NETWORK_PROTOCOL_NODE_ITEM_HPP_ */
