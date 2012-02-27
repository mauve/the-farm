/*
 * Copyright (C) 2012, All rights reserved, Mikael Olenfalk <mikael@olenfalk.se>
 */

#include "child_options.hpp"

#include <iostream>
#include <iterator>

namespace util {

namespace process {

child_options::child_options (const std::string& command)
	: _command(command), _use_env(false)
{}

child_options::~child_options ()
{}

child_options& child_options::add_arg(const std::string& arg)
{
	_args.push_back(arg);
	return *this;
}

child_options& child_options::set_args(const std::vector<std::string>& args)
{
	_args = args;
	return *this;
}

child_options& child_options::use_env(bool use)
{
	_use_env = use;
	return *this;
}

child_options& child_options::add_env(const std::string& key, const std::string& value)
{
	_env.insert (std::make_pair(key, value));
	return *this;
}

child_options& child_options::set_env(const std::map<std::string, std::string>& env)
{
	_env = env;
	return *this;
}

child_options& child_options::set_default_stream_behaviour(stream_behaviour beh)
{
	_streams[stream_id::standard_out] = beh;
	_streams[stream_id::standard_in] = beh;
	_streams[stream_id::standard_err] = beh;
	return *this;
}

child_options& child_options::set_stream_behaviour(stream_id stream, stream_behaviour beh)
{
	_streams.insert (std::make_pair (stream, beh));
	return *this;
}

const std::string& child_options::get_command () const
{
	return _command;
}

const std::vector<std::string>& child_options::get_args () const
{
	return _args;
}

const std::map<std::string, std::string>& child_options::get_env () const
{
	return _env;
}

const std::vector<std::string> child_options::get_formatted_env () const
{
	std::vector<std::string> formatted;
	for (std::map<std::string, std::string>::const_iterator iter = _env.begin();
				iter != _env.end(); ++iter)
	{
		formatted.push_back (iter->first + "=" + iter->second);
	}

	return formatted;
}

const std::map<stream_id, child_options::stream_behaviour>& child_options::get_stream_behaviours () const
{
	return _streams;
}

/*
 *
 */
std::ostream& operator<< (std::ostream& os, const child_options& opts)
{
	os << "cmd='" << opts.get_command() << "' ( ";
	std::copy(opts.get_args().begin(),
				opts.get_args().end(),
				std::ostream_iterator<std::string>(os, " "));
	os << ")";
	return os;
}

}  // namespace process

}  // namespace util


