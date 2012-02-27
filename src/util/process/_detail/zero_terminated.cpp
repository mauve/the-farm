/*
 * Copyright (C) 2012, All rights reserved, Mikael Olenfalk <mikael@olenfalk.se>
 */

#include "zero_teminated.hpp"

#include <cstring>

namespace util {

namespace process {

namespace _detail {

zero_terminated_list_of_strings::zero_terminated_list_of_strings()
{}

zero_terminated_list_of_strings::~zero_terminated_list_of_strings ()
{
	for (std::vector<char*>::iterator iter = _buf.begin();
			iter != _buf.end(); ++iter)
	{
		delete[] *iter;
	}
}

void zero_terminated_list_of_strings::add (const std::string& s)
{
	char* copy = new char[s.length() + 1];
	std::strcpy(copy, s.c_str());
	_buf.push_back(copy);
}

void zero_terminated_list_of_strings::add (const std::vector<std::string>& s)
{
	for (std::vector<std::string>::const_iterator iter = s.begin();
			iter != s.end(); ++iter)
	{
		add(*iter);
	}
}

char** zero_terminated_list_of_strings::data()
{
	if (_buf.empty() || _buf.back() != 0) {
		_buf.push_back(0);
	}

	return &*(_buf.begin());
}


}  // namespace _detail

}  // namespace process

}  // namespace util

