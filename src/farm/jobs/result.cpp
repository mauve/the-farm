/*
 * Copyright (C) 2012, All rights reserved, Mikael Olenfalk <mikael@olenfalk.se>
 */

#include "result.hpp"

#include <farm/compiler/error_info.hpp>
#include <farm/compiler/description.hpp>

#include <stdexcept>

namespace farm {

namespace jobs {

using std::string;
using std::vector;
using boost::uuids::uuid;

JobResult::JobResult (const uuid& agent_id,
						const CompilerDescription& compiler,
						const vector<ErrorInfo>& messages)
	: _agent_id(agent_id),
	  _compiler_description(compiler),
	  _messages(messages)
{}

JobResult::~JobResult ()
{}

const uuid& JobResult::agent_id () const
{
	return _agent_id;
}

const CompilerDescription& JobResult::compiler_description () const
{
	return _compiler_description;
}

bool JobResult::succeeded () const
{
	return _messages.empty();
}

const vector<ErrorInfo>& JobResult::messages () const
{
	return _messages;
}

vector<char>& JobResult::output (result_data_t res)
{
	return _outputs[res];
}

const vector<char>& JobResult::output (result_data_t res) const
{
	std::map<result_data_t, vector<char> >::const_iterator iter = _outputs.find(res);
	if (iter == _outputs.end())
		throw std::out_of_range("no such result");

	return iter->second;
}

}  // namespace jobs

}  // namespace farm

