/*
 * Copyright (C) 2012, All rights reserved, Mikael Olenfalk <mikael@olenfalk.se>
 */

#ifndef NETWORK_TRANSPORT_TCP_CONNECTION_HPP_
#define NETWORK_TRANSPORT_TCP_CONNECTION_HPP_

#include <deque>

#include <network/transport/connection.hpp>
#include <network/transport/message_queue.hpp>

#include <boost/cstdint.hpp>
#include <boost/asio.hpp>
#include <boost/signals2.hpp>
#include <boost/smart_ptr/shared_ptr.hpp>
#include <boost/smart_ptr/enable_shared_from_this2.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/noncopyable.hpp>
#include <boost/array.hpp>
#include <boost/function.hpp>

namespace network {

namespace transport {

class mock_connection :
	public boost::enable_shared_from_this2<mock_connection>,
	public connection
{
public:
	typedef boost::shared_ptr<mock_connection> pointer;

	static pointer create (boost::asio::io_service& io_service,
							boost::uint32_t maximum_payload_length);

	virtual void start ();
	virtual void stop ();

	virtual bool running () const;

	virtual void send (message::const_pointer msg);

	std::deque<message::const_pointer>& sent_messages ();
	void simulate_message_received (message::const_pointer msg);

private:
	bool _running () const;

	mock_connection (boost::asio::io_service& io_service,
				boost::uint32_t maximum_payload_length);

	boost::asio::ip::tcp::socket _socket;
	bool _started;
	bool _stopped;
	std::deque<message::const_pointer> _messages;
};

}  // namespace transport

}  // namespace network


#endif /* NETWORK_TRANSPORT_TCP_CONNECTION_HPP_ */
