/*
 * Copyright (C) 2012, All rights reserved, Mikael Olenfalk <mikael@olenfalk.se>
 */

#ifndef NETWORK_TRANSPORT_MESSAGE_QUEUE_HPP_
#define NETWORK_TRANSPORT_MESSAGE_QUEUE_HPP_

#include "message.hpp"

#include <list>
#include <boost/asio/buffer.hpp>
#include <boost/function.hpp>

namespace network {

namespace transport {

class message_queue
{
public:
	// return true if the queue was empty
	// before the add operation
	bool add (message::const_pointer msg);

	bool empty () const;

	void consume (std::size_t& num_bytes);

	boost::asio::const_buffer buffer () const;

private:
	struct item {
		enum { header, payload } state;
		message::header hdr;
		message::const_pointer msg;
		boost::uint32_t remains;

		item (message::const_pointer msg);

		boost::asio::const_buffer buffer () const;

		bool consume (std::size_t& num_bytes);
	};
	std::list<item> _queue;
};

}  // namespace transport

}  // namespace network


#endif /* NETWORK_TRANSPORT_MESSAGE_QUEUE_HPP_ */
