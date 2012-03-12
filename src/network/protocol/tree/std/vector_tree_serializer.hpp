/*
 * Copyright (C) 2012, All rights reserved, Mikael Olenfalk <mikael@olenfalk.se>
 */

#ifndef STANDARD_VECTOR_TREE_SERIALIZERS_HPP_
#define STANDARD_VECTOR_TREE_SERIALIZERS_HPP_

#include <network/protocol/tree/tree_serializer.hpp>

#include <vector>

namespace network {

namespace protocol {

namespace tree {

template <typename T>
struct tree_serializer<std::vector<T> >
{
public:
	const static boost::uint16_t node_id = 1;

	static boost::uint16_t length (const std::vector<T>& value)
	{
		return value.size();
	}

	static void serialize (const std::vector<T>& value, payload_output_stream& pos)
	{
		for (typename std::vector<T>::const_iterator iter = value.begin();
				iter != value.end(); ++iter)
		{
			serializer::serialize(*iter, pos);
		}
	}

	static std::vector<T> deserialize (boost::uint16_t length, payload_input_stream& pis)
	{
		std::vector<T> result(length);
		typename std::vector<T>::iterator iter = result.begin();
		while (length--)
			*iter++ = serializer::deserialize<T>(pis);
		return result;
	}
};

ENSURE_UNIQUE_NODEID(1);

}  // namespace tree

}  // namespace protocol

}  // namespace network

#endif /* STANDARD_VECTOR_TREE_SERIALIZERS_HPP_ */
