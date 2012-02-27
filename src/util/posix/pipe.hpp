/*
 * Copyright (C) 2012, All rights reserved, Mikael Olenfalk <mikael@olenfalk.se>
 */

#ifndef UTIL_POSIX_PIPE_HPP_
#define UTIL_POSIX_PIPE_HPP_

#include <boost/noncopyable.hpp>

namespace util {

namespace posix {

class pipe :
	public boost::noncopyable
{
public:
	pipe ();
	~pipe ();

	int read_end ();
	int steal_read_end ();

	int write_end ();
	int steal_write_end ();

private:
	int _read, _write;
};

}  // namespace posix

}  // namespace util

#endif /* UTIL_POSIX_PIPE_HPP_ */
