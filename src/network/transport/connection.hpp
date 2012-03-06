/*
 * Copyright (C) 2012, All rights reserved, Mikael Olenfalk <mikael@olenfalk.se>
 */

#ifndef NETWORK_TRANSPORT_CONNECTION_HPP_
#define NETWORK_TRANSPORT_CONNECTION_HPP_

#include <network/transport/message.hpp>

#include <boost/cstdint.hpp>
#include <boost/asio.hpp>
#include <boost/signals2.hpp>
#include <boost/smart_ptr/shared_ptr.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/noncopyable.hpp>
#include <boost/array.hpp>
#include <boost/function.hpp>

namespace network {

namespace transport {

class connection :
	public boost::noncopyable
{
public:
	typedef boost::shared_ptr<connection> pointer;
	typedef boost::signals2::signal<void (message::const_pointer msg)> on_message;
	typedef boost::signals2::signal<void ()> on_closed;

	virtual void start () = 0;
	virtual void stop () = 0;

	virtual bool running () const = 0;

	boost::uint32_t get_maximum_payload_length () const;
	boost::uint32_t set_maximum_payload_length (boost::uint32_t maximum_payload_length);

	virtual void send (message::const_pointer msg) = 0;

	boost::signals2::connection connect_on_message (const on_message::slot_type& cb);
	boost::signals2::connection connect_on_closed (const on_closed::slot_type& cb);

protected:
	connection (boost::asio::io_service& io_service,
				boost::uint32_t maximum_payload_length);

	boost::asio::io_service& _io_service;
	mutable boost::mutex _mutex;
	boost::uint32_t _maximum_payload_length;
	on_message _on_message;
	on_closed _on_closed;
};

}  // namespace transport

}  // namespace network


#endif /* NETWORK_TRANSPORT_CONNECTION_HPP_ */
