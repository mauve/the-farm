/*
 * Copyright (C) 2012, All rights reserved, Mikael Olenfalk <mikael@olenfalk.se>
 */

#ifndef NETWORK_TRANSPORT_MESSAGE_QUEUE_HPP_
#define NETWORK_TRANSPORT_MESSAGE_QUEUE_HPP_

#include "message.hpp"

#include <deque>
#include <boost/asio/buffer.hpp>
#include <boost/function.hpp>
#include <boost/noncopyable.hpp>

namespace network {

namespace transport {

class message_queue :
	public boost::noncopyable
{
public:
	message_queue ();
	~message_queue ();

	// return true if the queue was empty
	// before the add operation
	bool add (message::const_pointer msg);

	bool empty () const;

	void consume (std::size_t& num_bytes);

	boost::asio::const_buffer buffer () const;

private:
	struct item;
	std::deque<item> _queue;
};

}  // namespace transport

}  // namespace network


#endif /* NETWORK_TRANSPORT_MESSAGE_QUEUE_HPP_ */
