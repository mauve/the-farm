
#include <boost/test/unit_test.hpp>

#include <farm/compiler/configured_runner_config.hpp>
#include <farm/compiler/regex_error_parser.hpp>

struct Fixture
{
	boost::shared_ptr<farm::RegexErrorParser> error_parser;

	Fixture ()
	{
		std::vector<farm::RegexErrorParser::ParseSetting> parse_settings;
		std::vector<std::string> ignore_parsers;

		error_parser.reset(new farm::RegexErrorParser(parse_settings,
														ignore_parsers));
	}
};

BOOST_AUTO_TEST_SUITE(farm)

BOOST_AUTO_TEST_SUITE(compiler)

BOOST_FIXTURE_TEST_SUITE(configured_compiler_runner_config_tests, Fixture)

BOOST_AUTO_TEST_CASE(simple_test)
{
	std::vector<std::string> arguments;
	arguments.push_back("-o");
	arguments.push_back("${target-path}");
	arguments.push_back("${code-path}");

	ConfiguredCompilerRunnerConfig runner_config("/usr/bin/gcc",
													arguments,
													error_parser);

	BOOST_CHECK_EQUAL(runner_config.get_path(), "/usr/bin/gcc");

	std::vector<std::string> results =
			runner_config.get_arguments("hello.cpp", "hello");
	BOOST_CHECK_EQUAL(results.size(), 3);
	BOOST_CHECK_EQUAL(results[0], "-o");
	BOOST_CHECK_EQUAL(results[1], "hello");
	BOOST_CHECK_EQUAL(results[2], "hello.cpp");
}

BOOST_AUTO_TEST_SUITE_END() // configured_compiler_runner_config_tests

BOOST_AUTO_TEST_SUITE_END() // compiler

BOOST_AUTO_TEST_SUITE_END() // farm
