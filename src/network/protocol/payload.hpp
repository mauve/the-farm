/*
 * Copyright (C) 2012, All rights reserved, Mikael Olenfalk <mikael@olenfalk.se>
 */

#ifndef NETWORK_PROTOCOL_PAYLOAD_HPP_
#define NETWORK_PROTOCOL_PAYLOAD_HPP_

#include "node_item.hpp"

#include <vector>
#include <string>

#include <boost/variant.hpp>
#include <boost/cstdint.hpp>
#include <util/type_traits/remove_cv_ref.hpp>
#include <boost/mpl/contains.hpp>

namespace network {

namespace protocol {

// DO NOT CHANGE THE ORDER OF THESE
// ITEMS AND DO NOT ADD ITEMS IN THE
// MIDDLE, THE INDEX OF A TYPE IS
// SERIALIZED IN THE PAYLOAD
//	see: payload_serializer.cpp:
//		 	network::transport::_detail::
//				serialize_visitor::prefix_value
typedef boost::variant<
			bool,
			boost::int32_t,
			boost::uint32_t,
			boost::int64_t,
			boost::uint64_t,
			std::string,
			node_item,
			std::vector<boost::uint8_t>,
			std::vector<boost::int32_t>,
			std::vector<boost::uint32_t>,
			std::vector<boost::int64_t>,
			std::vector<boost::uint64_t>,
			std::vector<std::string>
		> payload_item_t;

typedef std::vector<payload_item_t> payload;

template <typename T>
struct is_payload_type :
	public boost::mpl::contains<
		payload_item_t::types,
		typename util::type_traits::remove_cv_ref< T >::type
	>
{};

}  // namespace protocol

}  // namespace network

#endif /* NETWORK_PROTOCOL_PAYLOAD_HPP_ */
