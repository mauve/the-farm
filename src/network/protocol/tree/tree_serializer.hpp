/*
 * Copyright (C) 2012, All rights reserved, Mikael Olenfalk <mikael@olenfalk.se>
 */

#ifndef NETWORK_PROTOCOL_TREE_TREE_SERIALIZER_HPP_
#define NETWORK_PROTOCOL_TREE_TREE_SERIALIZER_HPP_

#include <boost/cstdint.hpp>
#include <boost/mpl/bool.hpp>

#include <network/protocol/payload.hpp>

namespace network {

namespace protocol {

class payload_input_stream;
class payload_output_stream;

namespace tree {

/*
 * do not implement these methods,
 * you must specialize this thing
 */
template <typename T>
struct tree_serializer
{
	const static boost::uint16_t node_id;

	static boost::uint16_t length (const T& value);
	static void serialize (const T& value, payload_output_stream& pos);
	static T deserialize (boost::uint16_t length, payload_input_stream& pis);
};

template <typename T>
struct is_tree_serializable :
	public boost::mpl::false_
{};

struct serializer
{
	template <typename T>
	static inline
	typename boost::enable_if_c<
		is_tree_serializable<T>::value &&
		!is_payload_type<T>::value >::type
	serialize (const T& value, payload_output_stream& pos)
	{
		pos << node_item(tree_serializer<T>::node_id, tree_serializer<T>::length());;
		tree_serializer<T>::serialize(value, pos);
	}

	template <typename T>
	static inline
	typename boost::enable_if< is_payload_type<T> >::type
	serialize (const T& value, payload_output_stream& pos)
	{
		pos << value;
	}

	template <typename T>
	static inline
	typename boost::enable_if_c<
		is_tree_serializable<T>::value &&
		!is_payload_type<T>::value, T >::type
	deserialize (payload_input_stream& pis)
	{
		node_item ni = pis.read<node_item>();
		if (ni.type() != tree_serializer<T>::node_id)
			throw std::runtime_error("incompatible tree types");
		return tree_serializer<T>::deserialize(ni.length(), pis);
	}

	template <typename T>
	static inline
	typename boost::enable_if< is_payload_type<T>, T >::type
	deserialize (payload_input_stream& pis)
	{
		return pis.read<T>();
	}
};

#define MARK_TREE_SERIALIZABLE(TYPE) \
	template <> \
	struct is_tree_serializable<TYPE > : \
		public boost::mpl::true_ \
	{}

#define ENSURE_UNIQUE_NODEID(NODE_ID) \
	const int tree_serializer_Ensure_Unique__ ## NODE_ID = NODE_ID

#define DEFINE_TREE_SERIALIZER(TYPE, NODE_ID) \
	template <> \
	struct tree_serializer<TYPE > \
	{ \
		const static boost::uint16_t node_id; \
		static void serialize (const TYPE& value, payload_output_stream& pos); \
		static void deserialize (TYPE& value, payload_input_stream& pis); \
	}; \
	\
	ENSURE_UNIQUE_NODEID(NODE_ID)

}  // namespace tree

}  // namespace protocol

}  // namespace network

#endif /* NETWORK_PROTOCOL_TREE_TREE_SERIALIZER_HPP_ */
