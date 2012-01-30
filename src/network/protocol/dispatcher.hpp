/*
 * Copyright (C) 2012, All rights reserved, Mikael Olenfalk <mikael@olenfalk.se>
 */

#ifndef NETWORK_PROTOCOL_DISPATCHER_HPP_
#define NETWORK_PROTOCOL_DISPATCHER_HPP_

#include "util/functional/member-function-registry.hpp"
#include "_detail/arg_parser.hpp"
#include "_detail/return_parser.hpp"
#include "network/transport/message.hpp"
#include "network/transport/message_passer.hpp"

namespace network {

namespace protocol {

namespace _detail {

class base_dispatcher
{
public:
	base_dispatcher(transport::message_passer& mpi);

	transport::message::pointer call (boost::uint32_t opcode, transport::message::const_pointer in);

private:
	virtual void perform_call (transport::message_passer& mpi, boost::uint32_t opcode,
			return_parser& ret_parser, const payload& arguments) = 0;

	transport::message_passer& _mpi;
};

}  // namespace _detail

template <typename Class>
class dispatcher :
	public _detail::base_dispatcher
{
public:
	dispatcher (Class* instance, transport::message_passer& mpi)
		: _detail::base_dispatcher(mpi), _instance(instance)
	{}

	template <typename Function>
	void register_function (boost::uint32_t opcode, Function f)
	{
		_registry.register_function(opcode, f);
	}

	// the call function is inherited from
	// base_dispatcher

private:
	virtual void perform_call (transport::message_passer& mpi, boost::uint32_t opcode,
			_detail::return_parser& ret_parser, const payload& arguments)
	{
		_registry.call(_instance, mpi, opcode, ret_parser, arguments);
	}

	util::functional::member_function_registry<Class,
		_detail::arg_parser,
		payload,
		_detail::return_parser,
		transport::message_passer,
		boost::uint32_t> _registry;

	Class* _instance;
};

}  // namespace protocol

}  // namespace network


#endif /* NETWORK_PROTOCOL_DISPATCHER_HPP_ */
