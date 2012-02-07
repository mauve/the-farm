
#include "memstream.hpp"
#include <boost/test/unit_test.hpp>

struct MemoryOutputStreamFixture
{
	enum { size = 20 };

	char* data;
	util::memory_output_stream mos;

	MemoryOutputStreamFixture ()
		: data(new char[size]),
		  mos(data, size)
	{}

	~MemoryOutputStreamFixture ()
	{
		delete[] data;
	}
};

BOOST_AUTO_TEST_SUITE(util)

BOOST_FIXTURE_TEST_SUITE(memory_output_stream, MemoryOutputStreamFixture)

BOOST_AUTO_TEST_CASE(create)
{
	BOOST_CHECK(mos.remains() == size);
}

BOOST_AUTO_TEST_CASE(write)
{
	int i = ~0;
	BOOST_CHECK_NO_THROW(mos.write ((char*)&i, sizeof(int)));
	BOOST_CHECK_EQUAL(mos.remains(), size - sizeof(int));
	BOOST_CHECK_EQUAL(*((int*)data), i);
}

BOOST_AUTO_TEST_CASE(overflow)
{
	char buffer[size * 2];

	std::size_t remains_before = mos.remains();
	BOOST_CHECK_THROW(mos.write (buffer, size * 2), std::exception);
	BOOST_CHECK_EQUAL(mos.remains(), remains_before);
}

BOOST_AUTO_TEST_CASE(stream_operator)
{
	std::size_t remains_before = mos.remains();
	BOOST_CHECK_NO_THROW(mos << (~0));
	BOOST_CHECK_EQUAL(mos.remains(), size - sizeof(int));
	BOOST_CHECK_EQUAL(*((int*)data), ~0);
	BOOST_CHECK_EQUAL(remains_before - sizeof(int), mos.remains());
}

BOOST_AUTO_TEST_SUITE_END() // memory_output_stream

BOOST_AUTO_TEST_SUITE_END() // util



