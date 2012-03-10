/*
 * Copyright (C) 2012, All rights reserved, Mikael Olenfalk <mikael@olenfalk.se>
 */

#include <boost/test/unit_test.hpp>

#include <network/transport/message_passer.hpp>
#include <network/transport/mock_connection.hpp>

BOOST_AUTO_TEST_SUITE(network)

BOOST_AUTO_TEST_SUITE(transport)

BOOST_AUTO_TEST_SUITE(message_passer_tests)

struct reply_callback
{
	reply_callback ()
		: _has_been_called(false)
	{}

	bool has_been_called () const
	{
		return _has_been_called;
	}

	message::const_pointer reply () const
	{
		return _reply;
	}

	void operator() (message::const_pointer msg)
	{
		_has_been_called = true;
		_reply = msg;
	}

private:
	bool _has_been_called;
	message::const_pointer _reply;
};

struct subreply_callback
{
	subreply_callback ()
		: _has_been_called(false)
	{}

	bool has_been_called () const
	{
		return _has_been_called;
	}

	message::const_pointer reply () const
	{
		return _reply;
	}

	subscription_handle handle () const
	{
		return _handle;
	}

	void operator() (message::const_pointer msg, subscription_handle handle)
	{
		_has_been_called = true;
		_reply = msg;
		_handle = handle;
	}

private:
	bool _has_been_called;
	message::const_pointer _reply;
	subscription_handle _handle;
};

struct request_callback
{
	request_callback ()
		: _num_times_called(0)
	{}

	bool has_been_called () const
	{
		return !!_num_times_called;
	}

	int num_times_called () const
	{
		return _num_times_called;
	}

	message::pointer operator() (message::const_pointer req)
	{
		_num_times_called++;
		return message_passer::create_reply(req->get_header(), 0);
	}

private:
	int _num_times_called;
};

struct subrequest_callback
{
	subrequest_callback ()
		: _num_times_called(0), _disallow(false)
	{}

	bool has_been_called () const
	{
		return !!_num_times_called;
	}

	int num_times_called () const
	{
		return _num_times_called;
	}

	void disallow_requests ()
	{
		_disallow = true;
	}

	void allow_requests ()
	{
		_disallow = false;
	}

	message::error_codes operator() (message::const_pointer msg, subscription_sink sink)
	{
		_num_times_called++;
		if (_disallow)
			return message::error_invalid_subscription;
		_sink = sink;
		return message::error_success;
	}

	void produce (message::error_codes ec)
	{
		_sink.produce(ec);
	}

	void close_production ()
	{
		_sink.close();
	}

private:
	int _num_times_called;
	bool _disallow;
	subscription_sink _sink;
};

BOOST_AUTO_TEST_CASE(send_request_response)
{
	boost::asio::io_service io_service;
	mock_connection::pointer conn = mock_connection::create(io_service, 10000);
	message_passer::pointer mp = message_passer::create(conn);
	mp->start();

	reply_callback on_reply;
	mp->request(1, boost::ref(on_reply));

	BOOST_CHECK_EQUAL(conn->sent_messages().size(), 1);

	message::const_pointer msg = conn->sent_messages().back();
	BOOST_CHECK_EQUAL(msg->get_header().error_code, 0);
	BOOST_CHECK_EQUAL(msg->get_header().message_type, message::message_request);
	BOOST_CHECK_EQUAL(msg->get_header().opcode, 1);
	BOOST_CHECK_EQUAL(msg->get_header().payload_length, 0);

	message::pointer reply_msg =
			message::create(message::header(msg->get_header().conversation_id,
											msg->get_header().opcode,
											message::message_response, 0));
	conn->simulate_message_received(reply_msg);

	BOOST_CHECK(on_reply.has_been_called());
}

BOOST_AUTO_TEST_CASE(send_request_response_other_cid)
{
	boost::asio::io_service io_service;
	mock_connection::pointer conn = mock_connection::create(io_service, 10000);
	message_passer::pointer mp = message_passer::create(conn);
	mp->start();

	reply_callback on_reply;
	mp->request(1, boost::ref(on_reply));

	BOOST_CHECK_EQUAL(conn->sent_messages().size(), 1);

	message::const_pointer msg = conn->sent_messages().back();
	BOOST_CHECK_EQUAL(msg->get_header().error_code, 0);
	BOOST_CHECK_EQUAL(msg->get_header().message_type, message::message_request);
	BOOST_CHECK_EQUAL(msg->get_header().opcode, 1);
	BOOST_CHECK_EQUAL(msg->get_header().payload_length, 0);

	message::pointer reply_msg =
			message::create(message::header(msg->get_header().conversation_id + 1,
											msg->get_header().opcode,
											message::message_response, 0));
	conn->simulate_message_received(reply_msg);

	BOOST_CHECK(!on_reply.has_been_called());
}

BOOST_AUTO_TEST_CASE(subscribe)
{
	boost::asio::io_service io_service;
	mock_connection::pointer conn = mock_connection::create(io_service, 10000);
	message_passer::pointer mp = message_passer::create(conn);
	mp->start();

	reply_callback on_notification;
	subreply_callback on_subscription_reply;
	mp->subscribe(1, boost::ref(on_subscription_reply),
						boost::ref(on_notification));

	BOOST_CHECK_EQUAL(conn->sent_messages().size(), 1);

	message::const_pointer msg = conn->sent_messages().back();
	BOOST_CHECK_EQUAL(msg->get_header().error_code, 0);
	BOOST_CHECK_EQUAL(msg->get_header().message_type, message::message_subscribe_request);
	BOOST_CHECK_EQUAL(msg->get_header().opcode, 1);
	BOOST_CHECK_EQUAL(msg->get_header().payload_length, 0);

	message::pointer reply_msg =
			message::create(message::header(msg->get_header().conversation_id,
											msg->get_header().opcode,
											message::message_subscribe_response, 0));
	conn->simulate_message_received(reply_msg);

	BOOST_CHECK(on_subscription_reply.has_been_called());

	message::pointer notification_msg =
			message::create(message::header(msg->get_header().conversation_id,
											msg->get_header().opcode,
											message::message_notification, 0));
	conn->simulate_message_received(notification_msg);

	BOOST_CHECK(on_notification.has_been_called());
}

BOOST_AUTO_TEST_CASE(receive_request)
{
	boost::asio::io_service io_service;
	mock_connection::pointer conn = mock_connection::create(io_service, 10000);
	message_passer::pointer mp = message_passer::create(conn);
	mp->start();

	request_callback on_request;
	message_passer::callback_connection con =
			mp->connect_on_request(1, boost::ref(on_request));

	message::pointer req_msg =
			message::create(message::header(1, 2, message::message_request, 0));
	conn->simulate_message_received(req_msg);

	BOOST_CHECK(!on_request.has_been_called());

	req_msg = message::create(message::header(1, 1, message::message_request, 0));
	conn->simulate_message_received(req_msg);

	BOOST_CHECK(on_request.has_been_called());
	BOOST_CHECK_EQUAL(on_request.num_times_called(), 1);

	con.disconnect();
	conn->simulate_message_received(req_msg);

	BOOST_CHECK_EQUAL(on_request.num_times_called(), 1);
}

BOOST_AUTO_TEST_CASE(receive_sub_request)
{
	boost::asio::io_service io_service;
	mock_connection::pointer conn = mock_connection::create(io_service, 10000);
	message_passer::pointer mp = message_passer::create(conn);
	mp->start();

	subrequest_callback on_subrequest;
	message_passer::callback_connection con =
			mp->connect_on_subscription_request(1, boost::ref(on_subrequest));

	message::pointer req_msg =
			message::create(message::header(1, 1, message::message_subscribe_request, 0));
	conn->simulate_message_received(req_msg);

	BOOST_CHECK_EQUAL(conn->sent_messages().size(), 1);
	message::const_pointer msg = conn->sent_messages().back();
	BOOST_CHECK_EQUAL(msg->get_header().error_code, 0);
	BOOST_CHECK_EQUAL(msg->get_header().message_type, message::message_subscribe_response);
	BOOST_CHECK_EQUAL(msg->get_header().opcode, 1);
	BOOST_CHECK_EQUAL(msg->get_header().payload_length, 0);

	BOOST_CHECK(on_subrequest.has_been_called());
	BOOST_CHECK_EQUAL(on_subrequest.num_times_called(), 1);
}

BOOST_AUTO_TEST_CASE(receive_double_sub_request)
{
	boost::asio::io_service io_service;
	mock_connection::pointer conn = mock_connection::create(io_service, 10000);
	message_passer::pointer mp = message_passer::create(conn);
	mp->start();

	subrequest_callback on_subrequest;
	message_passer::callback_connection con =
			mp->connect_on_subscription_request(1, boost::ref(on_subrequest));

	message::pointer req_msg =
			message::create(message::header(1, 1, message::message_subscribe_request, 0));
	conn->simulate_message_received(req_msg);

	BOOST_CHECK_EQUAL(conn->sent_messages().size(), 1);
	message::const_pointer msg = conn->sent_messages().back();
	BOOST_CHECK_EQUAL(msg->get_header().error_code, 0);
	BOOST_CHECK_EQUAL(msg->get_header().message_type, message::message_subscribe_response);
	BOOST_CHECK_EQUAL(msg->get_header().opcode, 1);
	BOOST_CHECK_EQUAL(msg->get_header().payload_length, 0);

	BOOST_CHECK(on_subrequest.has_been_called());
	BOOST_CHECK_EQUAL(on_subrequest.num_times_called(), 1);

	/*
	 * send a second request for the same sub
	 */
	conn->sent_messages().clear();
	conn->simulate_message_received(req_msg);

	BOOST_CHECK_EQUAL(on_subrequest.num_times_called(), 1);
	BOOST_CHECK_EQUAL(conn->sent_messages().size(), 1);
	msg = conn->sent_messages().back();
	BOOST_CHECK_EQUAL(msg->get_header().error_code, message::error_invalid_subscription);
	BOOST_CHECK_EQUAL(msg->get_header().message_type, message::message_subscribe_response);
	BOOST_CHECK_EQUAL(msg->get_header().opcode, 1);
	BOOST_CHECK_EQUAL(msg->get_header().payload_length, 0);
}

BOOST_AUTO_TEST_CASE(receive_invalid_sub_request)
{
	boost::asio::io_service io_service;
	mock_connection::pointer conn = mock_connection::create(io_service, 10000);
	message_passer::pointer mp = message_passer::create(conn);
	mp->start();

	subrequest_callback on_subrequest;
	message_passer::callback_connection con =
			mp->connect_on_subscription_request(1, boost::ref(on_subrequest));

	message::pointer req_msg =
			message::create(message::header(1, 2, message::message_subscribe_request, 0));
	conn->simulate_message_received(req_msg);

	BOOST_CHECK(!on_subrequest.has_been_called());
	BOOST_CHECK_EQUAL(conn->sent_messages().size(), 1);
	message::const_pointer msg = conn->sent_messages().back();
	BOOST_CHECK_EQUAL(msg->get_header().error_code, message::error_invalid_subscription);
	BOOST_CHECK_EQUAL(msg->get_header().message_type, message::message_subscribe_response);
	BOOST_CHECK_EQUAL(msg->get_header().opcode, 2);
	BOOST_CHECK_EQUAL(msg->get_header().payload_length, 0);
}

BOOST_AUTO_TEST_CASE(receive_sub_request_and_disallow)
{
	boost::asio::io_service io_service;
	mock_connection::pointer conn = mock_connection::create(io_service, 10000);
	message_passer::pointer mp = message_passer::create(conn);
	mp->start();

	subrequest_callback on_subrequest;
	message_passer::callback_connection con =
			mp->connect_on_subscription_request(1, boost::ref(on_subrequest));

	message::pointer req_msg =
			message::create(message::header(1, 1, message::message_subscribe_request, 0));

	on_subrequest.disallow_requests();
	conn->simulate_message_received(req_msg);

	BOOST_CHECK(on_subrequest.has_been_called());
	BOOST_CHECK_EQUAL(conn->sent_messages().size(), 1);
	message::const_pointer msg = conn->sent_messages().back();
	BOOST_CHECK_EQUAL(msg->get_header().error_code, message::error_invalid_subscription);
	BOOST_CHECK_EQUAL(msg->get_header().message_type, message::message_subscribe_response);
	BOOST_CHECK_EQUAL(msg->get_header().opcode, 1);
	BOOST_CHECK_EQUAL(msg->get_header().payload_length, 0);
}

BOOST_AUTO_TEST_CASE(produce)
{
	boost::asio::io_service io_service;
	mock_connection::pointer conn = mock_connection::create(io_service, 10000);
	message_passer::pointer mp = message_passer::create(conn);
	mp->start();

	subrequest_callback on_subrequest;
	message_passer::callback_connection con =
			mp->connect_on_subscription_request(1, boost::ref(on_subrequest));

	message::pointer req_msg =
			message::create(message::header(1, 1, message::message_subscribe_request, 0));
	conn->simulate_message_received(req_msg);

	BOOST_CHECK_EQUAL(conn->sent_messages().size(), 1);
	message::const_pointer msg = conn->sent_messages().back();
	BOOST_CHECK_EQUAL(msg->get_header().error_code, 0);
	BOOST_CHECK_EQUAL(msg->get_header().message_type, message::message_subscribe_response);
	BOOST_CHECK_EQUAL(msg->get_header().opcode, 1);
	BOOST_CHECK_EQUAL(msg->get_header().payload_length, 0);

	BOOST_CHECK(on_subrequest.has_been_called());
	BOOST_CHECK_EQUAL(on_subrequest.num_times_called(), 1);

	conn->sent_messages().clear();

	on_subrequest.produce(message::error_success);

	BOOST_CHECK_EQUAL(conn->sent_messages().size(), 1);
	msg = conn->sent_messages().back();
	BOOST_CHECK_EQUAL(msg->get_header().error_code, 0);
	BOOST_CHECK_EQUAL(msg->get_header().message_type, message::message_notification);
	BOOST_CHECK_EQUAL(msg->get_header().opcode, 1);
	BOOST_CHECK_EQUAL(msg->get_header().payload_length, 0);

	/*
	 * check that close message is sent
	 */

	conn->sent_messages().clear();
	on_subrequest.close_production();

	BOOST_CHECK_EQUAL(conn->sent_messages().size(), 1);
	msg = conn->sent_messages().back();
	BOOST_CHECK_EQUAL(msg->get_header().error_code, message::error_operation_aborted);
	BOOST_CHECK_EQUAL(msg->get_header().message_type, message::message_notification);
	BOOST_CHECK_EQUAL(msg->get_header().opcode, 1);
	BOOST_CHECK_EQUAL(msg->get_header().payload_length, 0);
}

BOOST_AUTO_TEST_SUITE_END() // message_passer_tests

BOOST_AUTO_TEST_SUITE_END() // transpot

BOOST_AUTO_TEST_SUITE_END() // network

