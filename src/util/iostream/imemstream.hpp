
#ifndef UTIL_IOSTREAM_IMEMSTREAM_HPP_
#define UTIL_IOSTREAM_IMEMSTREAM_HPP_

#include <iostream>

namespace util {

class imembuf :
	public std::streambuf
{
public:
	imembuf (const char* begin, const char* end);
	explicit imembuf (const char* str);
	virtual ~imembuf ();

private:
	int_type underflow ();
	int_type uflow ();
	int_type pbackfail (int_type ch);
	std::streamsize showmanyc ();

	// copy not allowed
	imembuf (const imembuf&);
	imembuf& operator= (const imembuf&);

private:
	const char* const _begin;
	const char* const _end;
	const char* _current;
};

class imemstream :
	private virtual imembuf,
	public std::istream
{
public:
	imemstream (const char* begin, const char* end);
	explicit imemstream (const char* str);

	virtual ~imemstream ();
};

}  // namespace util

#endif /* UTIL_IOSTREAM_IMEMSTREAM_HPP_ */
