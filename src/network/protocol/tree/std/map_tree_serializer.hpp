/*
 * Copyright (C) 2012, All rights reserved, Mikael Olenfalk <mikael@olenfalk.se>
 */

#ifndef STANDARD_MAP_TREE_SERIALIZERS_HPP_
#define STANDARD_MAP_TREE_SERIALIZERS_HPP_

#include <network/protocol/tree/tree_serializer.hpp>
#include <network/protocol/tree/std/pair_tree_serializer.hpp>

#include <map>

namespace network {

namespace protocol {

namespace tree {

template <typename K, typename V>
struct tree_serializer<std::map<K, V> >
{
public:
	const static boost::uint16_t node_id = 2;

	static boost::uint16_t length (const std::map<K, V>& value)
	{
		return value.size();
	}

	static void serialize (const std::map<K, V>& value, payload_output_stream& pos)
	{
		for (std::map<K, V>::const_iterator iter = value.begin();
				iter != value.end(); ++iter)
		{
			serializer::serialize(*iter, pos);
		}
	}

	static std::map<K, V> deserialize (boost::uint16_t length, payload_input_stream& pis)
	{
		std::map<K, V> result;
		while (length--)
			result.insert(serializer::deserialize<std::pair<K, V> >(pis));
		return result;
	}
};

ENSURE_UNIQUE_NODEID(2);

}  // namespace tree

}  // namespace protocol

}  // namespace network

#endif /* STANDARD_MAP_TREE_SERIALIZERS_HPP_ */
