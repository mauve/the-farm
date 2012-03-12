/*
 * Copyright (C) 2012, All rights reserved, Mikael Olenfalk <mikael@olenfalk.se>
 */

#ifndef NETWORK_PROTOCOL_ARG_PARSER_HPP_
#define NETWORK_PROTOCOL_ARG_PARSER_HPP_

#include "../payload.hpp"

#include <boost/mpl/contains.hpp>
#include <boost/type_traits/remove_cv.hpp>
#include <boost/type_traits/remove_reference.hpp>

namespace network {

namespace protocol {

namespace _detail {

class arg_parser
{
public:
	arg_parser (const payload& p)
		: _payload(p), _iter(p.begin()), _end(p.end())
	{}

	template <typename Result>
	typename boost::enable_if<
		is_payload_type<Result>,
		Result >
	::type pop ()
	{
		if (_iter == _end)
			throw std::out_of_range("not enough arguments");
		return boost::get<Result>(*_iter++);
	}

private:
	const payload& _payload;
	payload::const_iterator _iter, _end;
};

}  // namespace _detail

}  // namespace protocol

}  // namespace network

#endif /* NETWORK_PROTOCOL_ARG_PARSER_HPP_ */
