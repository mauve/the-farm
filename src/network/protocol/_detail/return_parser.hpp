/*
 * Copyright (C) 2012, All rights reserved, Mikael Olenfalk <mikael@olenfalk.se>
 */

#ifndef NETWORK_PROTOCOL_DETAIL_RETURN_PARSER_HPP_
#define NETWORK_PROTOCOL_DETAIL_RETURN_PARSER_HPP_

#include <boost/mpl/contains.hpp>
#include <boost/utility/enable_if.hpp>

#include "../payload.hpp"

namespace network {

namespace protocol {

namespace _detail {

class return_parser
{
public:
	template <typename T>
	typename boost::enable_if< boost::mpl::contains<payload_item_t::types, T> >::type
	handle(const T& v)
	{
		_payload.push_back(v);
	}

	payload& result () const
	{
		return _payload;
	}

private:
	mutable payload _payload;
};

}  // namespace _detail

}  // namespace protocol

}  // namespace network

#endif /* NETWORK_PROTOCOL_DETAIL_RETURN_PARSER_HPP_ */
