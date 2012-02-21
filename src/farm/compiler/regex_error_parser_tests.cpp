
#include <boost/test/unit_test.hpp>

#include <farm/compiler/regex_error_parser.hpp>

BOOST_AUTO_TEST_SUITE(farm)

BOOST_AUTO_TEST_SUITE(compiler)

BOOST_AUTO_TEST_SUITE(regex_error_parser_tests)

BOOST_AUTO_TEST_CASE(no_patterns)
{
	std::vector<RegexErrorParser::ParseSetting> patterns;
	std::vector<std::string> ignore_patterns;

	RegexErrorParser parser(patterns, ignore_patterns);

	parser.parse_line("hello.cpp: 15: error: Declaration not found");

	std::vector<ErrorInfo> errors = parser.parsed_errors();

	BOOST_CHECK(errors.empty());
}

BOOST_AUTO_TEST_CASE(some_patterns_but_no_ignore_patterns)
{
	std::vector<RegexErrorParser::ParseSetting> patterns;
	std::vector<std::string> ignore_patterns;

	patterns.push_back(RegexErrorParser::ParseSetting(ErrorInfo::error,
			"(.*?):(\\d+):((\\d+):)? [Ee]rror: (.*)",
			1, 2, 4, 5));

	RegexErrorParser parser(patterns, ignore_patterns);

	parser.parse_line("hello.cpp:15:16: error: Message1");
	parser.parse_line("hello.cpp:16: error: Message1");

	std::vector<ErrorInfo> errors = parser.parsed_errors();

	BOOST_CHECK_EQUAL(errors.size(), 2);
	BOOST_CHECK_EQUAL(errors[0].get_file(), "hello.cpp");
	BOOST_CHECK_EQUAL(errors[0].get_level(), ErrorInfo::error);
	BOOST_CHECK_EQUAL(errors[0].get_line(), 15);
	BOOST_CHECK_EQUAL(errors[0].get_column(), 16);

	BOOST_CHECK_EQUAL(errors[1].get_file(), "hello.cpp");
	BOOST_CHECK_EQUAL(errors[1].get_level(), ErrorInfo::error);
	BOOST_CHECK_EQUAL(errors[1].get_line(), 16);
	BOOST_CHECK_EQUAL(errors[1].get_column(), -1);
}

BOOST_AUTO_TEST_CASE(ignore_patterns)
{
	std::vector<RegexErrorParser::ParseSetting> patterns;
	std::vector<std::string> ignore_patterns;

	patterns.push_back(RegexErrorParser::ParseSetting(ErrorInfo::error,
			"(.*?):(\\d+):(\\d+:)? [Ee]rror: (.*)",
			1, 2, 3, 4));

	ignore_patterns.push_back(".*Message1.*");

	RegexErrorParser parser(patterns, ignore_patterns);

	parser.parse_line("hello.cpp:15:16: error: Message1");

	std::vector<ErrorInfo> errors = parser.parsed_errors();

	BOOST_CHECK_EQUAL(errors.size(), 0);
}

BOOST_AUTO_TEST_SUITE_END() // regex_error_parser_tests

BOOST_AUTO_TEST_SUITE_END() // compiler

BOOST_AUTO_TEST_SUITE_END() // farm

