/*
 * Copyright (C) 2012, All rights reserved, Mikael Olenfalk <mikael@olenfalk.se>
 */

#ifndef NETWORK_TRANSPORT_SERIALIZE_HPP_
#define NETWORK_TRANSPORT_SERIALIZE_HPP_

#include "util/memstream.hpp"

namespace network {

namespace transport {

// must be specialized
template <typename Payload>
struct serializer
{
	serializer (const Payload& payload);

	std::size_t size () const;

	void serialize (util::memory_output_stream& mos) const;
};

// must be specialized
template <typename Payload>
struct deserializer
{
	static void deserialize (util::memory_input_stream& mis, Payload& p);
};

}  // namespace transport

}  // namespace network

#endif /* NETWORK_TRANSPORT_SERIALIZE_HPP_ */
