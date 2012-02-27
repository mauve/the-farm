/*
 * Copyright (C) 2012, All rights reserved, Mikael Olenfalk <mikael@olenfalk.se>
 */

#include "pipe.hpp"

#include <unistd.h>

#include <boost/system/error_code.hpp>
#include <boost/system/system_error.hpp>

namespace util {

namespace posix {

pipe::pipe ()
	: _read(-1), _write(-1)
{
	int pipes[2] = {0};
	if (::pipe(pipes) < 0)
	{
		throw boost::system::system_error(errno,
				boost::system::get_posix_category());
	}

	_read  = pipes[0];
	_write = pipes[1];
}

pipe::~pipe ()
{
	if (_read >= 0)
		::close(_read);
	if (_write >= 0)
		::close(_write);
}

int pipe::read_end()
{
	return _read;
}

int pipe::steal_read_end()
{
	int r = _read;
	_read = -1;
	return r;
}

int pipe::write_end()
{
	return _write;
}

int pipe::steal_write_end()
{
	int w = _write;
	_write = -1;
	return w;
}

}  // namespace posix

}  // namespace util
