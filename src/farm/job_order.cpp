/*
 * Copyright (C) 2012, All rights reserved, Mikael Olenfalk <mikael@olenfalk.se>
 */

#include "job_order.hpp"

#include "compiler.hpp"

namespace farm {

using std::string;
using std::vector;
using boost::uuids::uuid;

JobOrder::JobOrder (const uuid& id, const vector<CompilerRequirement>& reqs,
					const string& source_code, const string& input)
	: _id(id), _required_compilers(reqs), _code(source_code), _input(input)
{}

JobOrder::~JobOrder ()
{}

const uuid& JobOrder::get_id () const
{
	return _id;
}

const vector<CompilerRequirement>& JobOrder::required_compilers () const
{
	return _required_compilers;
}

const string& JobOrder::code () const
{
	return _code;
}

const string& JobOrder::input () const
{
	return _input;
}

}  // namespace farm


