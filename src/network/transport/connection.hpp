/*
 * Copyright (C) 2012, All rights reserved, Mikael Olenfalk <mikael@olenfalk.se>
 */

#ifndef NETWORK_TRANSPORT_CONNECTION_HPP_
#define NETWORK_TRANSPORT_CONNECTION_HPP_

#include "message.hpp"
#include "message_queue.hpp"

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

class connection :
	public boost::enable_shared_from_this2<connection>,
	public boost::noncopyable
{
public:
	typedef boost::shared_ptr<connection> pointer;
	typedef boost::signals2::signal<void (pointer, message::const_pointer msg)> on_message;
	typedef boost::signals2::signal<void (pointer)> on_closed;
	typedef boost::function<void (pointer, message::const_pointer)> on_message_sent_t;

	static pointer create (boost::asio::io_service& io_service,
							boost::uint32_t maximum_payload_length);

	boost::asio::ip::tcp::socket& socket ();

	void start ();
	void stop ();

	bool running () const;

	boost::uint32_t get_maximum_payload_length () const;
	boost::uint32_t set_maximum_payload_length (boost::uint32_t maximum_payload_length);

	void send (message::const_pointer msg);

	boost::signals2::connection connect_on_message (const on_message::slot_type& cb);
	boost::signals2::connection connect_on_closed (const on_closed::slot_type& cb);

private:
	void _handle_write (const boost::system::error_code& ec,
						std::size_t num_bytes);

	void _queue_read_header ();
	void _handle_read_header (const boost::system::error_code& ec,
								std::size_t num_bytes,
								boost::shared_ptr<message::header> hdr);
	void _queue_read_payload (const message::header& hdr);
	void _handle_read_payload (const boost::system::error_code& ec,
								std::size_t num_bytes,
								message::pointer msg);
	void _queue_ignore_payload (std::size_t num_bytes);

	typedef boost::shared_ptr<boost::array<boost::uint8_t, 1024> > ignore_buffer_t;
	void _handle_ignore_payload (const boost::system::error_code& ec,
									std::size_t num_bytes,
									ignore_buffer_t buffer,
									std::size_t remains);

private:
	// does not take the lock
	bool _running () const;

	connection (boost::asio::io_service& io_service,
				boost::uint32_t maximum_payload_length);

	boost::asio::io_service& _io_service;
	boost::asio::ip::tcp::socket _socket;

	on_message _on_message;
	on_closed _on_closed;

	mutable boost::mutex _mutex;
	boost::uint32_t _maximum_payload_length;
	bool _started;
	bool _stopped;
	message_queue _msg_queue;
};

}  // namespace transport

}  // namespace network


#endif /* NETWORK_TRANSPORT_CONNECTION_HPP_ */
