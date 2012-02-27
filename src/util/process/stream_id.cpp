/*
 * Copyright (C) 2012, All rights reserved, Mikael Olenfalk <mikael@olenfalk.se>
 */

#include "stream_id.hpp"

#include <cstdio>
#include <iostream>

namespace util {

namespace process {

const stream_id stream_id::standard_out (STDOUT_FILENO);
const stream_id stream_id::standard_in  (STDIN_FILENO);
const stream_id stream_id::standard_err (STDERR_FILENO);

stream_id::stream_id (native_stream_handle handle)
	: _handle(handle)
{}

stream_id::stream_id (const stream_id& copy)
	: _handle(copy._handle)
{}

stream_id::~stream_id ()
{}

stream_id& stream_id::operator= (const stream_id& oth)
{
	if (this != &oth)
		_handle = oth._handle;
	return *this;
}

stream_id::native_stream_handle stream_id::native () const
{
	return _handle;
}

bool operator== (const stream_id& l, const stream_id& r)
{
	return l.native() == r.native();
}

bool operator!= (const stream_id& l, const stream_id& r)
{
	return l.native() != r.native();
}

bool operator< (const stream_id& l, const stream_id& r)
{
	return l.native() < r.native();
}

std::ostream& operator<< (std::ostream& os, const stream_id& id)
{
	if (id == stream_id::standard_in)
		return os << id.native() << " (stdin)";
	if (id == stream_id::standard_out)
		return os << id.native() << " (stdout)";
	if (id == stream_id::standard_err)
		return os << id.native() << " (stderr)";
	return os << id.native();
}

}  // namespace process

}  // namespace util


