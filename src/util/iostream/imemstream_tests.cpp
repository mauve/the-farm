/*
 * Copyright (C) 2012, All rights reserved, Mikael Olenfalk <mikael@olenfalk.se>
 */

#include <boost/test/unit_test.hpp>

#include <util/iostream/imemstream.hpp>

#include <string>

BOOST_AUTO_TEST_SUITE(util)

BOOST_AUTO_TEST_SUITE(iostream)

BOOST_AUTO_TEST_SUITE(imemstream_tests)

BOOST_AUTO_TEST_CASE(simple_test)
{
	const char* data = "123\n456";
	std::string line;

	imemstream im(data, data + 7);

	BOOST_CHECK(std::getline(im, line));
	BOOST_CHECK_EQUAL(line, "123");

	BOOST_CHECK(std::getline(im, line));
	BOOST_CHECK_EQUAL(line, "456");
}

BOOST_AUTO_TEST_SUITE_END() // imemstream_tests

BOOST_AUTO_TEST_SUITE_END() // iostream

BOOST_AUTO_TEST_SUITE_END() // util
