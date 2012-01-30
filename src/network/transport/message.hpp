/*
 * Copyright (C) 2012, All rights reserved, Mikael Olenfalk <mikael@olenfalk.se>
 */

#ifndef NETWORK_TRANSPORT_MESSAGE_HPP_
#define NETWORK_TRANSPORT_MESSAGE_HPP_

#include <vector>
#include <boost/cstdint.hpp>
#include <boost/smart_ptr/shared_ptr.hpp>

namespace network {

namespace transport {

class message
{
public:
	typedef boost::shared_ptr<message> pointer;
	typedef boost::shared_ptr<const message> const_pointer;

	enum error_codes
	{
		error_success,
		error_invalid_operation,
		error_invalid_message_type,
		error_invalid_argument,
		error_invalid_payload,
		error_permission_denied,
		error_payload_too_large,
		error_operation_aborted,
		error_invalid_subscription,
		error_internal_error
	};

	enum message_types
	{
		message_request,
		message_response,
		message_subscribe_request,
		message_subscribe_response,
		message_unsubscribe_request,
		message_unsubscribe_response,
		message_notification
	};

	struct header
	{
		boost::uint32_t conversation_id;
		boost::uint32_t opcode;
		boost::uint32_t message_type;
		boost::uint32_t error_code;
		boost::uint32_t payload_length;

		header ();
		header (boost::uint32_t cid,
					boost::uint32_t op,
					boost::uint32_t mt,
					boost::uint32_t ec = 0);
	};

	static pointer create (const header& hdr);
	static pointer create (const header& hdr, boost::uint32_t payload_length);
	static pointer create (const header& hdr, const std::vector<boost::uint8_t>& payload);

	~message ();

	const header& get_header () const;
	void set_header (const header& hdr);

	boost::uint32_t get_payload_length () const;

	const boost::uint8_t* get_data () const;
	boost::uint8_t* get_data ();

protected:
	message (const header& hdr);
	message (const header& hdr, boost::uint32_t payload_length);
	message (const header& hdr, const std::vector<boost::uint8_t>& payload);

private:
	header _header;
	std::vector<boost::uint8_t> _payload;
};

}  // namespace transport

}  // namespace network


#endif /* NETWORK_TRANSPORT_MESSAGE_HPP_ */
