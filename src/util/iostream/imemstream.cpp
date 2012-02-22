
#include <util/iostream/imemstream.hpp>

#include <cstring>
#include <stdexcept>

namespace util {

imembuf::imembuf (const char* begin, const char* end)
	: _begin(begin), _end(end), _current(begin)
{
	if (_end < _begin)
		throw std::invalid_argument("_end must be reachable from _begin");
}

imembuf::imembuf (const char* str)
	: _begin(str),
	  _end(str + std::strlen(str)),
	  _current(str)
{}

imembuf::~imembuf()
{}

imembuf::int_type imembuf::underflow ()
{
	if (_current == _end)
		return traits_type::eof ();

	return traits_type::to_int_type(*_current);
}

imembuf::int_type imembuf::uflow ()
{
	if (_current == _end)
		return traits_type::eof ();

	return traits_type::to_int_type(*_current++);
}

imembuf::int_type imembuf::pbackfail (int_type ch)
{
	if (_current == _begin || (ch != traits_type::eof() && ch != _current[-1]))
		return traits_type::eof ();

	return traits_type::to_int_type(*--_current);
}

std::streamsize imembuf::showmanyc ()
{
	return _end - _current;
}

/*
 *
 */

imemstream::imemstream (const char* begin, const char* end)
	: imembuf(begin, end),
	  std::istream(this)
{}

imemstream::~imemstream()
{}

}  // namespace util
