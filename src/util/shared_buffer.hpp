
#ifndef UTIL_SHARED_BUFFER_HPP_
#define UTIL_SHARED_BUFFER_HPP_

#include <vector>
#include <boost/smart_ptr/shared_array.hpp>
#include <boost/asio/buffer.hpp>

namespace util {

//class shared_mutable_buffer
//{
//public:
//	explicit shared_mutable_buffer (std::size_t capacity);
//
//	const char* data () const;
//	std::size_t size () const;
//
//	boost::asio::mutable_buffer append_reserve (std::size_t required);
//	void report_appended (std::size_t size);
//#error PROCEED HERE
//
//private:
//	boost::shared_array<char> _data;
//	std::size_t _capacity, _size;
//};

// A reference-counted non-modifiable buffer class.
class shared_const_buffer
{
public:
	// Construct from a std::string.
	shared_const_buffer(const std::string& data)
		: _data(new char[data.length()]),
		  _buffer(_data.get(), data.length())
	{
		std::memcpy(_data.get(), data.c_str(), data.length());
	}

	explicit shared_const_buffer(const std::vector<char>& data)
		: _data(new char[data.length()]),
		  _buffer(_data.get(), data.length())
	{
		std::memcpy(_data.get(), &data[0], data.size());
	}

	// Implement the ConstBufferSequence requirements.
	typedef boost::asio::const_buffer value_type;
	typedef const boost::asio::const_buffer* const_iterator;
	const boost::asio::const_buffer* begin() const { return &_buffer; }
	const boost::asio::const_buffer* end() const { return &_buffer + 1; }

private:
	boost::shared_array<char> _data;
	boost::asio::const_buffer _buffer;
};

}  // namespace util


#endif /* UTIL_SHARED_BUFFER_HPP_ */
