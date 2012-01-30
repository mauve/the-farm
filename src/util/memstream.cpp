/*
 * Copyright (C) 2012, All rights reserved, Mikael Olenfalk <mikael@olenfalk.se>
 */

#include "memstream.hpp"

#include <stdexcept>

namespace util {

memory_output_stream::memory_output_stream (char* data, std::size_t size)
	: _begin(data), _current(data), _end(data + size)
{}

std::size_t memory_output_stream::remains () const
{
	return _end - _current;
}

void memory_output_stream::write (const char* data, std::size_t length)
{
	if (remains() < length)
		throw std::out_of_range("memory_output_stream exhausted");

	std::memcpy(_current, data, length);
	_current += length;
}

/*
 * memory_input_stream
 */
memory_input_stream::memory_input_stream (const char* data, std::size_t size)
	: _begin(data), _current(data), _end(data + size)
{}

std::size_t memory_input_stream::remains () const
{
	return _end - _current;
}

void memory_input_stream::read (char* data, std::size_t length)
{
	if (length > remains())
		throw std::out_of_range("memory_input_stream exhausted");

	std::memcpy(data, _current, length);
	_current += length;
}

const char* memory_input_stream::reserve (std::size_t length)
{
	if (length > remains())
		throw std::out_of_range("memory_input_stream exhausted");

	const char* region = _current;
	_current += length;
	return region;
}

}  // namespace util
