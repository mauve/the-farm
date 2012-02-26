
#ifndef UTIL_STUPIDTEST_STUPIDTEST_HPP_
#define UTIL_STUPIDTEST_STUPIDTEST_HPP_

#include <sstream>
#include <stdexcept>

/*
 * YOU ARE NOT SUPPOSED TO USE THIS
 */

namespace util {

namespace stupidtest {

void setup (const char* suite_name, bool verbose);

#define ST_RUN_TEST(x) \
	try { \
		util::stupidtest::_detail::report_case_start(#x); \
		x; \
		util::stupidtest::_detail::report_case_ended(#x, true); \
	} catch (const util::stupidtest::check_exception& ex) { \
		util::stupidtest::_detail::report_case_ended(#x, false, ex.what()); \
		throw; \
	} catch (const std::exception& ex) { \
		util::stupidtest::_detail::report_case_ended(#x, false, ex.what()); \
		throw; \
	}

#define ST_CHECK(x) \
	{ \
		util::stupidtest::_detail::report_check_before(#x); \
		if (!(x)) { \
			util::stupidtest::_detail::report_check_after(#x, false); \
			throw util::stupidtest::check_exception(#x); \
		} else { \
			util::stupidtest::_detail::report_check_after(#x, true); \
		} \
	}

#ifdef __GNUC__
#	define ST_TYPEOF(x) __typeof__(x)
#else
#	error Please port this.
#endif

#define ST_CHECK_OP(left, op, right) \
	{ \
		ST_TYPEOF(left) __left = left; \
		ST_TYPEOF(right) __right = right; \
		const char* const check_name = #left " " #op " " #right; \
		util::stupidtest::_detail::report_check_before(check_name); \
		if (!(__left op __right)) { \
			std::ostringstream oss; \
			oss << "[" << __left << " " #op " " << __right << "]"; \
			util::stupidtest::_detail::report_check_after(check_name, false, oss.str().c_str()); \
			throw util::stupidtest::check_exception(check_name); \
		} else { \
			util::stupidtest::_detail::report_check_after(check_name, true); \
		} \
	}

#define ST_CHECK_EQ(left, right) ST_CHECK_OP(left, ==, right)
#define ST_CHECK_NE(left, right) ST_CHECK_OP(left, !=, right)
#define ST_CHECK_LT(left, right) ST_CHECK_OP(left,  <, right)
#define ST_CHECK_GT(left, right) ST_CHECK_OP(left,  >, right)
#define ST_CHECK_LE(left, right) ST_CHECK_OP(left, <=, right)
#define ST_CHECK_GE(left, right) ST_CHECK_OP(left, >=, right)

class check_exception :
	public std::runtime_error
{
public:
	check_exception (const char* what)
		: std::runtime_error(what)
	{}
};

namespace _detail {

void report_case_start(const char* name);
void report_case_ended(const char* name, bool succeeded, const char* message = "");
void report_check_before(const char* check_name);
void report_check_after(const char* check_name, bool succeeded, const char* message = "");

}  // namespace _detail

}  // namespace stupidtest

}  // namespace util

#endif /* UTIL_STUPIDTEST_STUPIDTEST_HPP_ */
