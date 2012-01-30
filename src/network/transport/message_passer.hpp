/*
 * Copyright (C) 2012, All rights reserved, Mikael Olenfalk <mikael@olenfalk.se>
 */

#ifndef NETWORK_TRANSPORT_MESSAGE_PASSER_HPP_
#define NETWORK_TRANSPORT_MESSAGE_PASSER_HPP_

#include "connection.hpp"
#include "serialize.hpp"

#include <boost/function.hpp>
#include <boost/cstdint.hpp>
#include <boost/smart_ptr/enable_shared_from_this2.hpp>
#include <boost/smart_ptr/shared_ptr.hpp>
#include <boost/thread/mutex.hpp>

namespace network {

namespace transport {

class subscription_handle;
class subscription_sink;

namespace _detail {

struct subscription_handle_impl;
struct subscription_sink_impl;

}  // namespace _detail


class message_passer :
	public boost::enable_shared_from_this2<message_passer>
{
public:
	typedef boost::shared_ptr<message_passer> pointer;

	typedef boost::signals2::signal<message::pointer (message::const_pointer)> on_request;
	typedef boost::function<void (message::const_pointer msg)> on_reply;
	typedef boost::signals2::signal<void (message::const_pointer msg, subscription_sink)> on_subscription_request;
	typedef boost::function<void (message::const_pointer msg, subscription_handle)> on_subscription_reply;
	typedef on_reply on_notification;

	~message_passer ();

	static pointer create (connection::pointer);

	boost::signals2::connection connect_on_request (const on_request::slot_type& cb);
	boost::signals2::connection connect_on_subscription_request (const on_subscription_request::slot_type& cb);

	void request (boost::uint32_t opcode, const on_reply& cb);

	template <typename Payload>
	void request (boost::uint32_t opcode, const Payload& payload, const on_reply& cb);

	void subscribe (boost::uint32_t opcode,
					const on_subscription_reply& on_reply,
					const on_notification& on_data);

	/*
	 * helpers
	 */
	static message::header create_reply_header (const message::header& request, boost::uint32_t error_code);

	static message::pointer create_reply (const message::header& request, boost::uint32_t error_code);

	template <typename Payload>
	static message::pointer create_reply (const message::header& request, boost::uint32_t error_code, const Payload& payload);

private:
	friend struct _detail::subscription_handle_impl;
	friend struct _detail::subscription_sink_impl;

	void _do_request (message::const_pointer msg, const on_reply& cb);
	void _on_subscribe_reply (message::const_pointer msg, const on_subscription_reply& on_reply, const on_notification& on_data);
	void _request_sent (connection::pointer, message::const_pointer msg, const on_reply& cb);
	void _on_unsubscribe_reply (message::const_pointer msg);

	boost::uint32_t bake_cookie ();
	boost::uint32_t _bake_cookie (); // does not lock
	void _on_message_received (connection::pointer conn, message::const_pointer);

	void unsubscribe (boost::uint32_t opcode);
	void produce (message::const_pointer notification);
	void close_production (boost::uint32_t opcode, boost::uint32_t conversation_id);

	message_passer (connection::pointer);

	connection::pointer _connection;
	boost::signals2::scoped_connection _on_message_connection;

	on_request _on_request;
	on_subscription_request _on_subscription_request;

	mutable boost::mutex _mutex;
	boost::uint32_t _last_cookie;

	typedef std::map<boost::uint32_t, on_reply> request_map;
	request_map _outstanding_requests; // key is conversation_id
	typedef std::map<boost::uint32_t, on_notification> subscription_map;
	subscription_map _subscriptions; // key is opcode
	typedef std::map<boost::uint32_t, boost::weak_ptr<_detail::subscription_sink_impl> > productions_map;
	productions_map _productions; // key is opcode
};

class subscription_handle
{
public:
	subscription_handle ();
	~subscription_handle ();

	boost::uint32_t get_opcode () const;
	void unsubscribe ();

private:
	friend class message_passer;

	subscription_handle (message_passer::pointer owner, boost::uint32_t opcode);

	friend bool operator==(const subscription_handle& left, const subscription_handle& right);
	friend bool operator< (const subscription_handle& left, const subscription_handle& right);

private:
	boost::shared_ptr<_detail::subscription_handle_impl> _pimpl;
};

class subscription_sink
{
public:
	typedef boost::signals2::signal<void ()> on_unsubscribed;

	~subscription_sink ();

	boost::uint32_t get_opcode () const;
	boost::signals2::connection connect_on_unsubscribed (const on_unsubscribed::slot_type& cb);

	void produce (boost::uint32_t error_code);

	template <typename Payload>
	void produce (boost::uint32_t error_code, const Payload& payload);

	friend bool operator==(const subscription_sink& left, const subscription_sink& right);
	friend bool operator< (const subscription_sink& left, const subscription_sink& right);

	void close ();
	bool is_closed ();

private:
	friend class message_passer;

	subscription_sink (message_passer::pointer owner, boost::uint32_t opcode, boost::uint32_t conversation_id);

	// this extra indirection is required in order
	// to keep the _pimpl forward declared
	void produce (message::const_pointer msg);
	boost::uint32_t get_conversation_id () const;

private:
	boost::shared_ptr<_detail::subscription_sink_impl> _pimpl;
};

/*
 * message_passer
 */
template <typename Payload>
void message_passer::request(boost::uint32_t opcode, const Payload& payload, const on_reply& cb)
{
	boost::uint32_t cookie = bake_cookie ();

	message::header hdr(cookie, opcode, message::message_request);

	serializer<Payload> ser(payload);
	hdr.payload_length = ser.size();

	message::pointer msg = message::create(hdr);

	util::memory_output_stream mos(msg->get_data(), msg->get_payload_length());
	ser.serialize(mos);
	_do_request(msg, cb);
}

template <typename Payload>
message::pointer message_passer::create_reply(const message::header& request,
			boost::uint32_t error_code,
			const Payload& payload)
{
	message::header reply_hdr = create_reply_header(request, error_code);

	serializer<Payload> ser(payload);
	reply_hdr.payload_length = ser.size();

	message::pointer msg = message::create(reply_hdr);

	util::memory_output_stream mos(msg->get_data(), msg->get_payload_length());
	ser.serialize(mos);
	return msg;
}

/*
 * subscription_sink
 */

template <typename Payload>
void subscription_sink::produce(boost::uint32_t error_code, const Payload& payload)
{
	message::header hdr(get_conversation_id(),
						get_opcode(),
						message::message_notification,
						error_code);

	serializer<Payload> ser(payload);
	hdr.payload_length = ser.size();

	message::pointer msg = message::create(hdr);
	util::memory_output_stream mos(msg->get_data(), msg->get_payload_length());
	ser.serialize(mos);
	produce(msg);
}

}  // namespace transport

}  // namespace network


#endif /* NETWORK_TRANSPORT_MESSAGE_PASSER_HPP_ */
