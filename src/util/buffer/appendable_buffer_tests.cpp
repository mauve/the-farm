
#include <boost/test/unit_test.hpp>

#include <util/buffer/appendable_buffer.hpp>

BOOST_AUTO_TEST_SUITE(util)

BOOST_AUTO_TEST_SUITE(buffer_tests)

BOOST_AUTO_TEST_SUITE(appendable_buffer_tests)

BOOST_AUTO_TEST_CASE(create_invariant)
{
	buffer::appendable_buffer buf(10);
	BOOST_CHECK_GE(buf.capacity(), 10);
	BOOST_CHECK_EQUAL(buf.filled_size(), 0);
}

BOOST_AUTO_TEST_CASE(get_appendable_buffer)
{
	buffer::appendable_buffer buf(10);
	std::size_t initial_capacity = buf.capacity();

	boost::asio::mutable_buffer mbuf = buf.get_appendable_buffer(10);
	BOOST_CHECK(buf.capacity() == initial_capacity);
	BOOST_CHECK(buf.filled_size() == 0);
}

BOOST_AUTO_TEST_CASE(get_two_appendable_buffers)
{
	buffer::appendable_buffer buf(10);

	boost::asio::mutable_buffer mbuf1 = buf.get_appendable_buffer(10);
	boost::asio::mutable_buffer mbuf2 = buf.get_appendable_buffer(10);

	char* p1 = boost::asio::buffer_cast<char*>(mbuf1);
	char* p2 = boost::asio::buffer_cast<char*>(mbuf2);

	std::size_t s1 = boost::asio::buffer_size(mbuf1);
	std::size_t s2 = boost::asio::buffer_size(mbuf2);

	BOOST_CHECK(p1 == p2);
	BOOST_CHECK(s1 == s2);
	BOOST_CHECK(buf.filled_size() == 0);
}

BOOST_AUTO_TEST_CASE(append_buffer)
{
	buffer::appendable_buffer buf(20);

	boost::asio::mutable_buffer mbuf1 = buf.get_appendable_buffer(10);

	char* rbuf1 = boost::asio::buffer_cast<char*>(mbuf1);
	std::size_t rsize1 = boost::asio::buffer_size(mbuf1);

	std::fill(rbuf1, rbuf1 + rsize1, 'a');

	buf.report_appended(rsize1);
	BOOST_CHECK(buf.filled_size() == rsize1);
	BOOST_CHECK(rsize1 == 10);

	boost::asio::mutable_buffer mbuf2 = buf.get_appendable_buffer(10);

	char* rbuf2 = boost::asio::buffer_cast<char*>(mbuf2);
	std::size_t rsize2 = boost::asio::buffer_size(mbuf2);

	BOOST_CHECK(rbuf1 != rbuf2);
	BOOST_CHECK_EQUAL(rsize2, 10);
	BOOST_CHECK_NE(*rbuf2, 'a');
	BOOST_CHECK_EQUAL(rbuf2 - rbuf1, 10);
}

BOOST_AUTO_TEST_SUITE_END() // appendable_buffer_tests

BOOST_AUTO_TEST_SUITE_END() // buffer_tests

BOOST_AUTO_TEST_SUITE_END() // util

