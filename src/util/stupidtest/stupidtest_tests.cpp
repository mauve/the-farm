
#include <boost/test/unit_test.hpp>

#include <util/stupidtest/stupidtest.hpp>

struct Fixture
{
	std::ostringstream os;

	Fixture ()
	{
		util::stupidtest::setup("stupidtest", false);
	}

	~Fixture ()
	{
		os.str();
	}
};

void test1 ()
{
	ST_CHECK(1 == 1);
}

void test2 ()
{
	ST_CHECK(1 != 1);
}

BOOST_AUTO_TEST_SUITE(util)

BOOST_FIXTURE_TEST_SUITE(stupidtest_tests, Fixture)

BOOST_AUTO_TEST_CASE(check_ops)
{
	BOOST_CHECK_NO_THROW( ST_CHECK_EQ(1, 1) );
	BOOST_CHECK_NO_THROW( ST_CHECK_NE(1, 0) );
	BOOST_CHECK_NO_THROW( ST_CHECK_LT(1, 2) );
	BOOST_CHECK_NO_THROW( ST_CHECK_GT(2, 1) );
	BOOST_CHECK_NO_THROW( ST_CHECK_LE(1, 2) );
	BOOST_CHECK_NO_THROW( ST_CHECK_LE(1, 1) );
	BOOST_CHECK_NO_THROW( ST_CHECK_GE(2, 1) );
	BOOST_CHECK_NO_THROW( ST_CHECK_GE(1, 1) );

	BOOST_CHECK_THROW( ST_CHECK_EQ(1, 0), std::exception );
	BOOST_CHECK_THROW( ST_CHECK_NE(1, 1), std::exception );
	BOOST_CHECK_THROW( ST_CHECK_LT(2, 2), std::exception );
	BOOST_CHECK_THROW( ST_CHECK_GT(2, 3), std::exception );
	BOOST_CHECK_THROW( ST_CHECK_LE(3, 2), std::exception );
	BOOST_CHECK_THROW( ST_CHECK_GE(2, 3), std::exception );
}

BOOST_AUTO_TEST_CASE(run_test)
{
	BOOST_CHECK_NO_THROW( ST_RUN_TEST( test1() ) );
	BOOST_CHECK_THROW( ST_RUN_TEST( test2() ), std::exception );
}

BOOST_AUTO_TEST_SUITE_END() // stupidtest_tests

BOOST_AUTO_TEST_SUITE_END() // util
