/*
 * Copyright (C) 2012, All rights reserved, Mikael Olenfalk <mikael@olenfalk.se>
 */

#include "message_passer.hpp"

#include <boost/bind.hpp>
#include <boost/thread/locks.hpp>

namespace network {

namespace transport {

using boost::unique_lock;
using boost::recursive_mutex;

namespace _detail {

struct subscription_handle_impl
{
	subscription_handle_impl (message_passer::pointer owner, boost::uint32_t opcode)
		: _owner(owner), _opcode(opcode)
	{}

	~subscription_handle_impl ()
	{
		unsubscribe ();
	}

	void unsubscribe ()
	{
		message_passer::pointer owner = _owner.lock();
		if (!owner)
			return;

		owner->unsubscribe(_opcode);
	}

	boost::weak_ptr<message_passer> _owner;
	boost::uint32_t _opcode;
};

struct subscription_sink_impl
{
public:
	subscription_sink_impl (message_passer::pointer owner, boost::uint32_t opcode, boost::uint32_t conversation_id)
		: _owner(owner), _opcode(opcode),
		  _conversation_id(conversation_id), _closed(false)
	{
	}

	~subscription_sink_impl ()
	{
		close_production ();
	}

	void close_production ()
	{
		unique_lock<recursive_mutex> lock(_mutex);
		if (_closed)
			return;

		message_passer::pointer owner = _owner.lock ();
		if (!owner)
			return;

		_closed = true;
		owner->close_production(_opcode, _conversation_id);
	}

	bool is_closed ()
	{
		unique_lock<recursive_mutex> lock(_mutex);
		return _closed;
	}

	void raise_unsubscribe ()
	{
		_on_unsubscribed();
	}

	void produce (message::const_pointer msg)
	{
		message_passer::pointer owner = _owner.lock();
		if (!owner || is_closed())
			return;

		owner->produce(msg);
	}

	boost::weak_ptr<message_passer> _owner;
	boost::uint32_t _opcode;
	boost::uint32_t _conversation_id;
	subscription_sink::on_unsubscribed _on_unsubscribed;
	boost::recursive_mutex _mutex;
	bool _closed;
};

}  // namespace _detail

/*
 * message_passer
 */

message_passer::pointer message_passer::create (connection::pointer conn)
{
	return pointer(new message_passer(conn));
}

message_passer::message_passer (connection::pointer conn)
	: _connection(conn), _last_cookie(0)
{
	if (!_connection)
		throw std::invalid_argument("connection cannot be null");
}

message_passer::~message_passer()
{}

void message_passer::start ()
{
	// TODO: probably requires protection against double-start
	_connection->start ();

	_on_message_connection = _connection->connect_on_message(boost::bind(&message_passer::_on_message_received,
																			shared_from_this(),
																			_1));

}

message_passer::callback_connection message_passer::connect_on_request(boost::uint32_t opcode, const on_request& cb)
{
	return _request_handlers.connect(opcode, cb);
}

message_passer::callback_connection message_passer::connect_on_subscription_request(boost::uint32_t opcode, const on_subscription_request& cb)
{
	return _subscription_handlers.connect(opcode, cb);
}

void message_passer::request (boost::uint32_t opcode, const on_reply& cb)
{
	boost::uint32_t cookie = bake_cookie ();

	message::header hdr(cookie, opcode,
						message::message_request);
	_do_request (message::create(hdr), cb);
}

void message_passer::subscribe (boost::uint32_t opcode,
								const on_subscription_reply& on_reply,
								const on_notification& on_data)
{
	boost::uint32_t cookie = 0;
	{
		unique_lock<recursive_mutex> lock(_mutex);
		if (_subscriptions.find(opcode) != _subscriptions.end())
			throw std::invalid_argument ("double subscription");

		cookie = _bake_cookie ();
	}

	message::header hdr(cookie, opcode,
						message::message_subscribe_request);
	_do_request (message::create(hdr),
					boost::bind(&message_passer::_on_subscribe_reply,
								shared_from_this(),
								_1,
								on_reply,
								on_data));
}

message::header message_passer::create_reply_header(const message::header& request, boost::uint32_t error_code)
{
	message::header reply_hdr;
	reply_hdr.conversation_id = request.conversation_id;
	reply_hdr.opcode = request.opcode;
	reply_hdr.error_code = error_code;
	reply_hdr.payload_length = 0;

	switch (request.message_type)
	{
	case message::message_request:
		reply_hdr.message_type = message::message_response;
		break;

	case message::message_subscribe_request:
		reply_hdr.message_type = message::message_subscribe_response;
		break;

	case message::message_unsubscribe_request:
		reply_hdr.message_type = message::message_unsubscribe_response;
		break;

	default:
		throw std::invalid_argument("can only create a response for a request-type");
	}

	return reply_hdr;
}

message::pointer message_passer::create_reply(const message::header& request, boost::uint32_t error_code)
{
	return message::create(create_reply_header(request, error_code));
}

void message_passer::_do_request (message::const_pointer msg, const on_reply& cb)
{
	_connection->send(msg);

	unique_lock<recursive_mutex> lock(_mutex);
	_outstanding_requests.insert(std::make_pair(msg->get_header().conversation_id, cb));
}

void message_passer::_on_subscribe_reply(message::const_pointer msg,
											const on_subscription_reply& on_reply,
											const on_notification& on_data)
{
	boost::uint32_t error_code = msg->get_header().error_code;
	boost::uint32_t opcode = msg->get_header().opcode;

	if (error_code != message::error_success)
	{
		on_reply(msg, subscription_handle());
		return;
	}

	{
		unique_lock<recursive_mutex> lock(_mutex);
		_subscriptions.insert(std::make_pair(opcode, on_data));
	}

	on_reply(msg, subscription_handle(shared_from_this(), opcode));
}

void message_passer::_request_sent (connection::pointer, message::const_pointer msg, const on_reply& cb)
{
}

boost::uint32_t message_passer::bake_cookie ()
{
	unique_lock<recursive_mutex> lock(_mutex);
	return _bake_cookie ();
}

boost::uint32_t message_passer::_bake_cookie ()
{
	return ++_last_cookie;
}

void message_passer::_on_message_received(message::const_pointer msg)
{
	const message::header& hdr = msg->get_header();
	switch (hdr.message_type)
	{
		case message::message_notification: {
			unique_lock<recursive_mutex> lock(_mutex);
			subscription_map::iterator iter = _subscriptions.find(hdr.opcode);
			if (iter == _subscriptions.end())
				return;

			iter->second(msg);
			break;
		}

		case message::message_request: {
			unique_lock<recursive_mutex> lock(_mutex);
			message::pointer reply;
			try {
				reply = _request_handlers.get_callback(hdr.opcode) (msg);
			} catch (const util::functional::no_such_callback&) {
				reply = create_reply(hdr, message::error_invalid_operation);
			} catch (const std::exception& ex) {
				reply = create_reply(hdr, message::error_internal_error);
			}

			try {
				_connection->send(reply);
			} catch (const std::exception& ex) {
				try {
					_connection->send(create_reply(hdr, message::error_internal_error));
				} catch (...) {
					// at this point there is no recovery,
					// so let's just ignore any errors
				}
			}
			break;
		}

		case message::message_subscribe_request: {
			{
				unique_lock<recursive_mutex> lock(_mutex);
				if (_productions.find(hdr.opcode) != _productions.end()) {
					_connection->send(create_reply(hdr, message::error_invalid_subscription));
					return;
				}

				message::pointer reply;
				try {
					subscription_sink sink(shared_from_this(), hdr.opcode, hdr.conversation_id);
					message::error_codes ec = _subscription_handlers.get_callback(hdr.opcode) (msg, sink);

					if (ec == message::error_success) {
						_productions.insert(std::make_pair(hdr.opcode, sink._pimpl));
						reply = create_reply(hdr, message::error_success);
					} else {
						reply = create_reply(hdr, ec);
					}
				} catch (const util::functional::no_such_callback&) {
					reply = create_reply(hdr, message::error_invalid_subscription);
				} catch (const std::exception& ex) {
					reply = create_reply(hdr, message::error_internal_error);
				}
				_connection->send(reply);
			}
			break;
		}

		case message::message_unsubscribe_request: {
			{
				unique_lock<recursive_mutex> lock(_mutex);
				productions_map::iterator iter = _productions.find (hdr.opcode);
				if (iter != _productions.end()) {
					boost::shared_ptr<_detail::subscription_sink_impl> sink = iter->second.lock();
					_productions.erase(iter);
					if (sink) {
						sink->raise_unsubscribe();
					}
				}
			}

			_connection->send(create_reply(hdr, message::error_success));

			break;
		}

		case message::message_unsubscribe_response:
		case message::message_subscribe_response:
			// technically this design enables
			// the message_(un)subscribe_response and
			// message_response message-types to be
			// used interchangeably, we do not
			// care just now
		case message::message_response: {
			unique_lock<recursive_mutex> lock(_mutex);
			request_map::iterator iter = _outstanding_requests.find(hdr.conversation_id);
			if (iter == _outstanding_requests.end())
				return;

			request_map::value_type iter_val(*iter);
			_outstanding_requests.erase(iter);
			iter_val.second(msg);
			break;
		}

		default:
			// just ignore for now
			return;
	}
}

void message_passer::unsubscribe(boost::uint32_t opcode)
{
	boost::uint32_t cookie = 0;
	{
		unique_lock<recursive_mutex> lock(_mutex);
		_subscriptions.erase(opcode);
		cookie = _bake_cookie();
	}

	message::header hdr(cookie, opcode,
						message::message_unsubscribe_request);
	_do_request (message::create(hdr),
					boost::bind(&message_passer::_on_unsubscribe_reply,
								shared_from_this(), _1));

}

void message_passer::produce(message::const_pointer msg)
{
	_connection->send(msg);
}

void message_passer::close_production(boost::uint32_t opcode, boost::uint32_t conversation_id)
{
	unique_lock<recursive_mutex> lock(_mutex);
	productions_map::iterator iter = _productions.find(opcode);
	if (iter == _productions.end())
		return;

	_productions.erase(opcode);

	message::header hdr(conversation_id,
						opcode,
						message::message_notification,
						message::error_operation_aborted);
	_connection->send(message::create(hdr));
}

void message_passer::_on_unsubscribe_reply (message::const_pointer)
{
	// do nothing
}

/*
 * subscription_handle
 */

subscription_handle::subscription_handle()
{}

subscription_handle::subscription_handle(message_passer::pointer owner, boost::uint32_t opcode)
	: _pimpl(new _detail::subscription_handle_impl(owner, opcode))
{
}

subscription_handle::~subscription_handle()
{}

void subscription_handle::unsubscribe()
{
	if (_pimpl)
		_pimpl->unsubscribe();
}

boost::uint32_t subscription_handle::get_opcode() const
{
	return _pimpl ? _pimpl->_opcode : 0;
}

bool operator==(const subscription_handle& left, const subscription_handle& right)
{
	return left._pimpl == right._pimpl;
}

bool operator< (const subscription_handle& left, const subscription_handle& right)
{
	return left._pimpl < right._pimpl;
}

/*
 * subscription_sink
 */

subscription_sink::subscription_sink()
{}

subscription_sink::subscription_sink(message_passer::pointer owner, boost::uint32_t opcode, boost::uint32_t conversation_id)
	: _pimpl(new _detail::subscription_sink_impl(owner, opcode, conversation_id))
{}

subscription_sink::~subscription_sink()
{}

boost::uint32_t subscription_sink::get_opcode () const
{
	if (!_pimpl)
		return 0;
	return _pimpl->_opcode;
}

boost::uint32_t subscription_sink::get_conversation_id () const
{
	if (!_pimpl)
		return 0;
	return _pimpl->_conversation_id;
}

boost::signals2::connection subscription_sink::connect_on_unsubscribed(const on_unsubscribed::slot_type& cb)
{
	if (!_pimpl)
		throw std::invalid_argument("sink closed");
	return _pimpl->_on_unsubscribed.connect(cb);
}

void subscription_sink::produce(boost::uint32_t error_code)
{
	if (!_pimpl)
		throw std::invalid_argument("sink closed");
	message::header hdr(_pimpl->_conversation_id,
						_pimpl->_opcode,
						message::message_notification,
						error_code);
	produce(message::create(hdr));
}

void subscription_sink::produce(message::const_pointer msg)
{
	if (!_pimpl)
		throw std::invalid_argument("sink closed");
	_pimpl->produce(msg);
}

void subscription_sink::close ()
{
	if (!_pimpl)
		return;
	_pimpl->close_production();
}

bool subscription_sink::is_closed ()
{
	if (!_pimpl)
		return true;
	return _pimpl->is_closed();
}

bool operator==(const subscription_sink& left, const subscription_sink& right)
{
	return left._pimpl == right._pimpl;
}

bool operator< (const subscription_sink& left, const subscription_sink& right)
{
	return left._pimpl < right._pimpl;
}

/*
 * simple_production
 */
simple_production::simple_production (message_passer::pointer mp, boost::uint32_t opcode)
	: _opcode(opcode)
{
	_con = mp->connect_on_subscription_request(
					opcode,
					boost::bind(&simple_production::_on_subscribe,
							this, _1, _2));
}

simple_production::~simple_production()
{}

boost::uint32_t simple_production::get_opcode() const
{
	return _opcode;
}

message::error_codes simple_production::_on_subscribe(message::const_pointer msg, subscription_sink sink)
{
	_sink = sink;
	return message::error_success;
}

void simple_production::produce(boost::uint32_t error_code)
{
	if (_sink.is_closed())
		return;
	_sink.produce(error_code);
}

}  // namespace transport

}  // namespace network
