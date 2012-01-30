/*
 * Copyright (C) 2012, All rights reserved, Mikael Olenfalk <mikael@olenfalk.se>
 */

#include "dispatcher.hpp"

namespace network {

namespace protocol {

namespace _detail {

using transport::message;
using transport::message_passer;

message::pointer base_dispatcher::call (boost::uint32_t opcode, message::const_pointer in)
{
	payload arguments;
	try {
		if (in->get_payload_length()) {
			util::memory_input_stream mis((const char*)in->get_data(), in->get_payload_length());
			transport::deserializer<payload> deserializer;
			deserializer.deserialize(mis, arguments);
		}
	} catch (...) {
		// deserialization failed
		return message_passer::create_reply(in->get_header(), message::error_invalid_payload);
	}

	try {
		return_parser ret_parser;
		perform_call(_mpi, opcode, ret_parser, arguments);

		transport::serializer<payload> serializer(ret_parser.result());
		message::header reply_header = message_passer::create_reply_header(in->get_header(), message::error_success);
		message::pointer reply = message::create(reply_header, serializer.size());

		util::memory_output_stream mos((char*)reply->get_data(), reply->get_payload_length());
		serializer.serialize(mos);

		return reply;
	} catch (...) {
		// at this point we want to do some proper error
		// mapping instead
		// TODO: add correct error mapping
		return message_passer::create_reply(in->get_header(), message::error_internal_error);
	}
}

}  // namespace _detail

}  // namespace protocol

}  // namespace network
