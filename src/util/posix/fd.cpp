
#include <util/posix/fd.hpp>
#include <iostream>

namespace util {

namespace posix {

fd::fd (int fdnum)
	: _fd(fdnum)
{}

fd::fd ()
	: _fd(-1)
{}

fd::~fd ()
{
	close ();
}

bool fd::valid () const
{
	return _fd >= 0;
}

boost::system::error_code fd::close ()
{
	if (_fd < 0)
		return boost::system::errc::success;

	return boost::system::error_code(::close(_fd),
			boost::system::generic_category());
}

void fd::assign (int fdnum)
{
	(void) close();
	_fd = fdnum;
}

int fd::get ()
{
	return _fd;
}

int fd::steal ()
{
	int ret = _fd;
	_fd = -1;
	return ret;
}

fd& fd::operator= (int fdnum)
{
	assign (fdnum);
	return *this;
}

bool operator== (const fd& l, const fd& r)
{
	return l.get() == r.get();
}

bool operator!= (const fd& l, const fd& r)
{
	return l.get() != r.get();
}

bool operator<  (const fd& l, const fd& r)
{
	return l.get() < r.get();
}

std::ostream& operator<< (std::ostream& os, const fd& f)
{
	return os << f.get();
}

}  // namespace posix

}  // namespace util
