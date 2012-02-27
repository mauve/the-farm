/*
 * Copyright (C) 2012, All rights reserved, Mikael Olenfalk <mikael@olenfalk.se>
 */

#include <boost/test/unit_test.hpp>
#include "zero_teminated.hpp"

using util::process::_detail::zero_terminated_list_of_strings;

BOOST_AUTO_TEST_SUITE(util)

BOOST_AUTO_TEST_SUITE(process)

BOOST_AUTO_TEST_SUITE(_detail)

BOOST_AUTO_TEST_SUITE(zero_terminated_tests)

BOOST_AUTO_TEST_CASE(empty)
{
	zero_terminated_list_of_strings los;
	char** result = los.data ();

	BOOST_CHECK(result);
	BOOST_CHECK(result[0] == 0);
}

BOOST_AUTO_TEST_CASE(add_string)
{
	zero_terminated_list_of_strings los;

	BOOST_CHECK_NO_THROW(los.add("Hello"));

	char** result = los.data ();
	BOOST_CHECK(result);
	BOOST_CHECK_EQUAL(result[0], std::string("Hello"));
	BOOST_CHECK(result[1] == 0);
}

BOOST_AUTO_TEST_CASE(add_many_strings)
{
	zero_terminated_list_of_strings los;

	std::vector<std::string> strings;
	strings.push_back("Hello");
	strings.push_back("World");

	BOOST_CHECK_NO_THROW(los.add(strings));

	char** result = los.data ();
	BOOST_CHECK(result);
	BOOST_CHECK_EQUAL(result[0], std::string("Hello"));
	BOOST_CHECK_EQUAL(result[1], std::string("World"));
	BOOST_CHECK(result[2] == 0);
}

BOOST_AUTO_TEST_SUITE_END() // zero_terminated

BOOST_AUTO_TEST_SUITE_END() // _detail

BOOST_AUTO_TEST_SUITE_END() // process

BOOST_AUTO_TEST_SUITE_END() // util

