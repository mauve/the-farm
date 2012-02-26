
#ifndef UTIL_POSIX_FD_HPP_
#define UTIL_POSIX_FD_HPP_

#include <iosfwd>

#include <boost/noncopyable.hpp>
#include <boost/system/error_code.hpp>

namespace util {

namespace posix {

class fd :
	public boost::noncopyable
{
public:
	explicit fd (int fdnum);
	fd ();
	~fd ();

	bool valid () const;

	boost::system::error_code close ();
	void assign (int fdnum);
	int get ();
	int steal ();

	fd& operator= (int fdnum);

private:
	int _fd;
};

bool operator== (const fd& l, const fd& r);
bool operator!= (const fd& l, const fd& r);
bool operator<  (const fd& l, const fd& r);

std::ostream& operator<< (std::ostream& os, const fd& f);

}  // namespace posix

}  // namespace util

#endif /* UTIL_POSIX_FD_HPP_ */
