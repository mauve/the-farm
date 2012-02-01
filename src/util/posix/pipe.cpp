
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
	::close(_read);
	::close(_write);
}

int pipe::read_end()
{
	return _read;
}

int pipe::write_end()
{
	return _write;
}

}  // namespace posix

}  // namespace util
