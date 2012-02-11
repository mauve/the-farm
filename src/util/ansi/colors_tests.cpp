
#include "colors.hpp"
#include <boost/test/unit_test.hpp>

namespace {

util::ansi::color_t colors[] = {
	util::ansi::black,
	util::ansi::red,
	util::ansi::green,
	util::ansi::yellow,
	util::ansi::blue,
	util::ansi::magenta,
	util::ansi::cyan,
	util::ansi::white,
	util::ansi::ignore,
	util::ansi::default_setting
};
int num_colors = sizeof(colors) / sizeof(colors[0]);

}

BOOST_AUTO_TEST_SUITE(util)

BOOST_AUTO_TEST_SUITE(ansi_colors_tests)

BOOST_AUTO_TEST_CASE(no_facet)
{
	std::ostringstream oss;
	oss << util::ansi::color_setting(util::ansi::green, util::ansi::red);

	BOOST_CHECK(oss.str() == "");
}

BOOST_AUTO_TEST_CASE(disabled_facet)
{
	std::locale loc(std::locale(), new util::ansi::color_facet(false));
	std::ostringstream oss;
	oss.imbue(loc);
	oss << util::ansi::color_setting(util::ansi::green, util::ansi::red);

	BOOST_CHECK(oss.str() == "");
}

BOOST_AUTO_TEST_CASE(only_foreground_colors)
{
	const char* expected[] = {
		"\x1b[30m", // black
		"\x1b[31m", // red
		"\x1b[32m", // green
		"\x1b[33m", // yellow
		"\x1b[34m", // blue
		"\x1b[35m", // magenta
		"\x1b[36m", // cyan
		"\x1b[37m", // white
		"", // ignore
		"\x1b[39m", // default_setting
	};

	for (int i = 0; i < num_colors; ++i) {
		std::locale loc(std::locale(), new util::ansi::color_facet(true));
		std::ostringstream oss;
		oss.imbue(loc);

		oss << util::ansi::color_setting(colors[i]);

		BOOST_CHECK_EQUAL(oss.str(), expected[i]);
	}
}

BOOST_AUTO_TEST_CASE(only_background_colors)
{
	const char* expected[] = {
		"\x1b[40m", // black
		"\x1b[41m", // red
		"\x1b[42m", // green
		"\x1b[43m", // yellow
		"\x1b[44m", // blue
		"\x1b[45m", // magenta
		"\x1b[46m", // cyan
		"\x1b[47m", // white
		"", // ignore
		"\x1b[49m", // default_setting
	};

	for (int i = 0; i < num_colors; ++i) {
		std::locale loc(std::locale(), new util::ansi::color_facet(true));
		std::ostringstream oss;
		oss.imbue(loc);

		oss << util::ansi::color_setting(util::ansi::ignore, colors[i]);

		BOOST_CHECK_EQUAL(oss.str(), expected[i]);
	}
}

BOOST_AUTO_TEST_CASE(reset_all)
{
	std::locale loc(std::locale(), new util::ansi::color_facet(true));
	std::ostringstream oss;
	oss.imbue(loc);

	oss << util::ansi::reset_all;

	BOOST_CHECK_EQUAL(oss.str(), "\x1b[0;0m");
}

BOOST_AUTO_TEST_CASE(predefined_foreground_helpers)
{
	using namespace util::ansi;

	const char* expected[] = {
		"\x1b[30m", // black
		"\x1b[31m", // red
		"\x1b[32m", // green
		"\x1b[33m", // yellow
		"\x1b[34m", // blue
		"\x1b[35m", // magenta
		"\x1b[36m", // cyan
		"\x1b[37m", // white
		"\x1b[39m", // default_setting
	};

	const color_setting* colors[] = {
		&fg_black,
		&fg_red,
		&fg_green,
		&fg_yellow,
		&fg_blue,
		&fg_magenta,
		&fg_cyan,
		&fg_white,
		&fg_default_setting
	};
	int num_colors = sizeof(colors) / sizeof(colors[0]);

	for (int i = 0; i < num_colors; ++i) {
		std::locale loc(std::locale(), new util::ansi::color_facet(true));
		std::ostringstream oss;
		oss.imbue(loc);

		oss << *(colors[i]);

		BOOST_CHECK_EQUAL(oss.str(), expected[i]);
	}
}

BOOST_AUTO_TEST_CASE(predefined_background_helpers)
{
	using namespace util::ansi;

	const char* expected[] = {
		"\x1b[40m", // black
		"\x1b[41m", // red
		"\x1b[42m", // green
		"\x1b[43m", // yellow
		"\x1b[44m", // blue
		"\x1b[45m", // magenta
		"\x1b[46m", // cyan
		"\x1b[47m", // white
		"\x1b[49m", // default_setting
	};

	const color_setting* colors[] = {
		&bg_black,
		&bg_red,
		&bg_green,
		&bg_yellow,
		&bg_blue,
		&bg_magenta,
		&bg_cyan,
		&bg_white,
		&bg_default_setting
	};
	int num_colors = sizeof(colors) / sizeof(colors[0]);

	for (int i = 0; i < num_colors; ++i) {
		std::locale loc(std::locale(), new util::ansi::color_facet(true));
		std::ostringstream oss;
		oss.imbue(loc);

		oss << *(colors[i]);

		BOOST_CHECK_EQUAL(oss.str(), expected[i]);
	}
}

BOOST_AUTO_TEST_SUITE_END() // ansi_colors_tests

BOOST_AUTO_TEST_SUITE_END() // util


