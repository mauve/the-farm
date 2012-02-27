/*
 * Copyright (C) 2012, All rights reserved, Mikael Olenfalk <mikael@olenfalk.se>
 */

#ifndef UTIL_PROCESS__DETAIL_ZERO_TEMINATED_HPP_
#define UTIL_PROCESS__DETAIL_ZERO_TEMINATED_HPP_

#include <vector>
#include <string>

namespace util {

namespace process {

namespace _detail {

class zero_terminated_list_of_strings
{
public:
	zero_terminated_list_of_strings ();
	~zero_terminated_list_of_strings ();

	void add (const std::string& s);
	void add (const std::vector<std::string>& s);

	char** data();

private:
	std::vector<char*> _buf;
};


}  // namespace _detail

}  // namespace process

}  // namespace util

#endif /* UTIL_PROCESS__DETAIL_ZERO_TEMINATED_HPP_ */
