/*
 * Copyright (C) 2012, All rights reserved, Mikael Olenfalk <mikael@olenfalk.se>
 */

#ifndef NETWORK_TRANSPORT_TCP_CONNECTION_HPP_
#define NETWORK_TRANSPORT_TCP_CONNECTION_HPP_

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

class tcp_connection :
	public boost::enable_shared_from_this2<tcp_connection>,
	public connection
{
public:
	typedef boost::shared_ptr<tcp_connection> pointer;

	static pointer create (boost::asio::io_service& io_service,
							boost::uint32_t maximum_payload_length);

	virtual void start ();
	virtual void stop ();

	virtual bool running () const;

	boost::asio::ip::tcp::socket& socket ();

	virtual void send (message::const_pointer msg);

private:
	bool _running () const;

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
	tcp_connection (boost::asio::io_service& io_service,
				boost::uint32_t maximum_payload_length);

	boost::asio::ip::tcp::socket _socket;
	bool _started;
	bool _stopped;
	message_queue _msg_queue;
};

}  // namespace transport

}  // namespace network


#endif /* NETWORK_TRANSPORT_TCP_CONNECTION_HPP_ */
