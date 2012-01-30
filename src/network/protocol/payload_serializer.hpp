/*
 * Copyright (C) 2012, All rights reserved, Mikael Olenfalk <mikael@olenfalk.se>
 */

#ifndef NETWORK_PROTOCOL_PAYLOAD_SERIALIZER_HPP_
#define NETWORK_PROTOCOL_PAYLOAD_SERIALIZER_HPP_

#include "network/transport/serialize.hpp"
#include "network/protocol/payload.hpp"

namespace network {

namespace transport {

template <>
class serializer<network::protocol::payload>
{
public:
	serializer (const network::protocol::payload& payload);

	std::size_t size () const;

	void serialize (util::memory_output_stream& mos) const;

private:
	const network::protocol::payload& _payload;
};

template <>
class deserializer<network::protocol::payload>
{
public:
	static void deserialize(util::memory_input_stream& mis,
							network::protocol::payload& payload);
};

}  // namespace transport

}  // namespace network

#endif /* NETWORK_PROTOCOL_PAYLOAD_SERIALIZER_HPP_ */
