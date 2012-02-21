/*
 * Copyright (C) 2012, All rights reserved, Mikael Olenfalk <mikael@olenfalk.se>
 */

#include <farm/jobs/order.hpp>
#include <farm/compiler/description.hpp>

namespace farm {

namespace jobs {

using std::string;
using std::vector;
using boost::uuids::uuid;

JobOrder::JobOrder (const uuid& id)
	: _id(id)
{}

JobOrder::JobOrder (const uuid& id,
					const vector<CompilerDescription>& compilers,
					const string& source_code,
					const string& input)
	: _id(id), _compilers(compilers), _code(source_code), _input(input)
{}

JobOrder::~JobOrder ()
{}

const uuid& JobOrder::get_id () const
{
	return _id;
}

vector<CompilerDescription>& JobOrder::get_compilers ()
{
	return _compilers;
}

const vector<CompilerDescription>& JobOrder::get_compilers () const
{
	return _compilers;
}

const string& JobOrder::get_code () const
{
	return _code;
}

const string& JobOrder::get_input () const
{
	return _input;
}

}  // namespace jobs

}  // namespace farm


