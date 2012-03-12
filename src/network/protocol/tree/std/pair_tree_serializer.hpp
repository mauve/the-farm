
#ifndef STANDARD_PAIR_TREE_SERIALIZERS_HPP_
#define STANDARD_PAIR_TREE_SERIALIZERS_HPP_

#include <network/protocol/tree/tree_serializer.hpp>
#include <network/protocol/tree/std/pair_tree_serializer.hpp>

#include <map>

namespace network {

namespace protocol {

namespace tree {

template <typename K, typename V>
struct tree_serializer<std::pair<K, V> >
{
public:
	const static boost::uint16_t node_id = 3;

	static boost::uint16_t length (const std::pair<K, V>&)
	{
		return 2;
	}

	static void serialize (const std::pair<K, V>& value, payload_output_stream& pos)
	{
		serializer::serialize(value.first, pos);
		serializer::serialize(value.second, pos);
	}

	static std::pair<K, V> deserialize (boost::uint16_t length, payload_input_stream& pis)
	{
		K k = serializer::deserialize<K> (pis);
		V v = serializer::deserialize<V> (pis);
		return std::pair<K, V>(k, v);
	}
};

ENSURE_UNIQUE_NODEID(3);

}  // namespace tree

}  // namespace protocol

}  // namespace network

#endif /* STANDARD_PAIR_TREE_SERIALIZERS_HPP_ */
