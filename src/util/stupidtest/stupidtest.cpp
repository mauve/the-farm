
#include <util/stupidtest/stupidtest.hpp>

#include <cstdlib>
#include <iostream>

namespace util {

namespace stupidtest {

namespace {

const char* g_suite_name = 0;
bool g_verbose = false;
bool g_under_teamcity = false;

void report_suite_ended ()
{
	if (g_under_teamcity) {
		std::cerr << "##teamcity[testSuiteEnded name='" << g_suite_name << "']" << std::endl;
	} else {
		std::cerr << "...test suite ended." << std::endl;
	}
}

}  // namespace


void setup (const char* suite_name, bool verbose)
{
	std::cerr << std::flush;

	g_suite_name = suite_name;
	g_verbose = verbose;
	g_under_teamcity = !!std::getenv("TEAMCITY_PROJECT_NAME");

	std::atexit(&report_suite_ended);

	if (g_under_teamcity) {
		std::cerr << "##teamcity[testSuiteStarted name='" << g_suite_name << "']" << std::endl;
	} else {
		std::cerr << "Starting Test Suite: " << g_suite_name << std::endl;
	}
}

namespace _detail {

void report_case_start(const char* name)
{
	if (g_under_teamcity) {
		std::cerr << "##teamcity[testStarted name='" << name << "' captureStandardOutput='true']" << std::endl;
	} else {
		std::cerr << "  Running test case " << name << ": " << std::endl;
	}
}

void report_case_ended(const char* name, bool succeeded, const char* message)
{
	if (g_under_teamcity) {
		if (!succeeded)
			std::cerr << "##teamcity[testFailed name='" << name << "' message='" << message << "']" << std::endl;
		std::cerr << "##teamcity[testEnded name='" << name << "']" << std::endl;
	} else {
		if (!succeeded)
			std::cerr << "  ..failed (message: " << message << ")" << std::endl;
		else
			std::cerr << "  ..succeeded" << std::endl;
	}
}

void report_check_before(const char* name)
{
	if (g_under_teamcity)
		return;

	std::cerr << "     Running check: " << name << ": " << std::flush;
}

void report_check_after(const char* name, bool succeeded, const char* message /* = 0 */)
{
	if (g_under_teamcity) {
		if (succeeded)
			return;

		std::cerr << "##teamcity[testFailed name='" << name << "' message='" << message << "']" << std::endl;
	} else {
		if (!succeeded)
			std::cerr << "failed (message: " << message << ")" << std::endl;
		else
			std::cerr << "succeeded" << std::endl;
	}
}

}  // namespace _detail

}  // namespace stupidtest

}  // namespace util
