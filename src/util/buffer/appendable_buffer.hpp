/*
 * Copyright (C) 2012, All rights reserved, Mikael Olenfalk <mikael@olenfalk.se>
 */

#ifndef UTIL_BUFFER_APPENDABLE_BUFFER_HPP_
#define UTIL_BUFFER_APPENDABLE_BUFFER_HPP_

#include <vector>
#include <boost/asio/buffer.hpp>

namespace util {

namespace buffer {

class appendable_buffer
{
public:
	appendable_buffer ();
	explicit appendable_buffer (std::size_t initial_size);

	std::size_t capacity () const;
	std::size_t filled_size () const;
	void reset ();
	void move_to (std::vector<char>& result);

	const char* begin () const;
	const char* end () const;

	boost::asio::mutable_buffers_1 get_appendable_buffer (std::size_t buf_size);
	void report_appended (std::size_t num_filled);

private:
	std::vector<char> _buffer;
	std::size_t _filled_size;
};

}  // namespace buffer

}  // namespace util

#endif /* UTIL_BUFFER_APPENDABLE_BUFFER_HPP_ */
