/*
 * Copyright (C) 2012, All rights reserved, Mikael Olenfalk <mikael@olenfalk.se>
 */

#ifndef UTIL_MEMSTREAM_HPP_
#define UTIL_MEMSTREAM_HPP_

#include <cstddef>
#include <boost/utility/enable_if.hpp>
#include <boost/type_traits/is_pod.hpp>

namespace util {

/*
 * memory_output_stream
 */

class memory_output_stream
{
public:
	memory_output_stream (char* data, std::size_t size);

	std::size_t remains () const;

	void write (const char* data, std::size_t length);

private:
	char* _begin;
	char* _current;
	char* _end;
};

template <typename T>
inline typename boost::enable_if<
	boost::is_pod<T>,
	memory_output_stream&
>::type
operator<< (memory_output_stream& mos, const T& v)
{
	mos.write ((const char*)&v, sizeof(v));
	return mos;
}

/*
 * memory_input_stream
 */

class memory_input_stream
{
public:
	memory_input_stream (const char* data, std::size_t size);

	std::size_t remains () const;

	void read (char* data, std::size_t length);

	const char* reserve (std::size_t length);

	template <typename T>
	inline typename boost::enable_if<
		boost::is_pod<T>,
		T
	>::type extract ()
	{
		T v;
		read ((char*)&v, sizeof(T));
		return v;
	}

private:
	const char* _begin;
	const char* _current;
	const char* _end;
};

template <typename T>
inline typename boost::enable_if<
	boost::is_pod<T>,
	memory_input_stream&
>::type
operator>> (memory_input_stream& mis, T& v)
{
	mis.read((char*)&v, sizeof(v));
	return mis;
}

}  // namespace util

#endif /* UTIL_MEMSTREAM_HPP_ */
