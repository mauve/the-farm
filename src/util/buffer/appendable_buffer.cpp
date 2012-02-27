/*
 * Copyright (C) 2012, All rights reserved, Mikael Olenfalk <mikael@olenfalk.se>
 */

#include <util/buffer/appendable_buffer.hpp>

namespace util {

namespace buffer {

appendable_buffer::appendable_buffer ()
	: _filled_size(0)
{}

appendable_buffer::appendable_buffer (std::size_t initial_size)
	: _buffer(initial_size), _filled_size(0)
{}

std::size_t appendable_buffer::capacity () const
{
	return _buffer.size ();
}

std::size_t appendable_buffer::filled_size () const
{
	return _filled_size;
}

void appendable_buffer::reset ()
{
	std::vector<char>().swap(_buffer);
	_filled_size = 0;
}

void appendable_buffer::move_to (std::vector<char>& result)
{
	_buffer.swap (result);
	result.resize(_filled_size);
	_filled_size = 0;
}

const char* appendable_buffer::begin () const
{
	return &*_buffer.begin ();
}

const char* appendable_buffer::end () const
{
	return &*_buffer.end ();
}

boost::asio::mutable_buffers_1 appendable_buffer::get_appendable_buffer (std::size_t buf_size)
{
	if ((_buffer.size() - _filled_size) < buf_size)
		_buffer.resize(_buffer.size() + buf_size);

	return boost::asio::mutable_buffers_1(&_buffer[0] + _filled_size, buf_size);
}

void appendable_buffer::report_appended (std::size_t num_filled)
{
	_filled_size += num_filled;
	if (_filled_size > _buffer.size())
		_filled_size = _buffer.size();
}

}  // namespace buffer

}  // namespace util
