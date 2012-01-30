/*
 * Copyright (C) 2012, All rights reserved, Mikael Olenfalk <mikael@olenfalk.se>
 */

#ifndef NETWORK_PROTOCOL_PAYLOAD_STREAM_HPP_
#define NETWORK_PROTOCOL_PAYLOAD_STREAM_HPP_

#include <network/protocol/payload.hpp>

#include <boost/mpl/contains.hpp>
#include <boost/type_traits/is_same.hpp>
#include <boost/utility/enable_if.hpp>

namespace network {

namespace protocol {

class payload_input_stream
{
public:
	payload_input_stream (const payload& p);
	~payload_input_stream ();

	bool empty () const;

	void reset ();

	template <typename T>
	typename boost::enable_if<
		boost::mpl::contains<payload_item_t::types, T>,
		T const &
	>::type
	read ()
	{
		return boost::get<T>(get());
	}

	template <typename T>
	typename boost::enable_if<
		boost::is_same<payload_item_t, T>,
		T const &
	>::type
	read ()
	{
		return get();
	}

private:
	const payload_item_t& get ();
	payload::const_iterator _begin, _iter, _end;
};

template <typename T>
payload_input_stream& operator>> (payload_input_stream& pis, T& t)
{
	t = pis.template read<T> ();
	return pis;
}

/*
 * payload_output_stream
 */
class payload_output_stream
{
public:
	payload_output_stream (payload& p);
	~payload_output_stream ();

	template <typename T>
	void write (const T& t)
	{
		_payload.push_back (t);
	}

private:
	payload& _payload;
};

template <typename T>
payload_output_stream& operator<< (payload_output_stream& pos, const T& t)
{
	pos.template write<T> (t);
	return pos;
}

}  // namespace protocol

}  // namespace network

#endif /* NETWORK_PROTOCOL_PAYLOAD_STREAM_HPP_ */
