
#include <boost/test/unit_test.hpp>

#include <farm/requirements/matchers.hpp>

BOOST_AUTO_TEST_SUITE(farm)

BOOST_AUTO_TEST_SUITE(requirements)

BOOST_AUTO_TEST_SUITE(matchers_tests)

BOOST_AUTO_TEST_CASE(EqualMatcher)
{
	matchers::EqualMatcher m;

	BOOST_CHECK(m.supports("=="));
	BOOST_CHECK(m.matches("a", "==", "a"));
	BOOST_CHECK(!m.matches("a", "==", "b"));
}

BOOST_AUTO_TEST_CASE(NotEqualMatcher)
{
	matchers::NotEqualMatcher m;

	BOOST_CHECK(m.supports("!="));
	BOOST_CHECK(!m.matches("a", "!=", "a"));
	BOOST_CHECK(m.matches("a", "!=", "b"));
}

BOOST_AUTO_TEST_CASE(LessMatcher)
{
	matchers::LessMatcher m;

	BOOST_CHECK(m.supports("<"));
	BOOST_CHECK(m.matches("a", "<", "b"));
	BOOST_CHECK(!m.matches("b", "<", "a"));
}

BOOST_AUTO_TEST_CASE(GreaterMatcher)
{
	matchers::GreaterMatcher m;

	BOOST_CHECK(m.supports(">"));
	BOOST_CHECK(m.matches("b", ">", "a"));
	BOOST_CHECK(!m.matches("a", ">", "b"));
}

BOOST_AUTO_TEST_CASE(LessEqualMatcher)
{
	matchers::LessEqualMatcher m;

	BOOST_CHECK(m.supports("<="));
	BOOST_CHECK(m.matches("a", "<=", "a"));
	BOOST_CHECK(m.matches("a", "<=", "b"));
	BOOST_CHECK(!m.matches("b", "<=", "a"));
}

BOOST_AUTO_TEST_CASE(GreaterEqualMatcher)
{
	matchers::GreaterEqualMatcher m;

	BOOST_CHECK(m.supports(">="));
	BOOST_CHECK(m.matches("a", ">=", "a"));
	BOOST_CHECK(!m.matches("a", ">=", "b"));
	BOOST_CHECK(m.matches("b", ">=", "a"));
}

BOOST_AUTO_TEST_CASE(HasMatcher)
{
	matchers::HasMatcher m;

	BOOST_CHECK(m.supports("has"));
	BOOST_CHECK(m.matches("a", "has", ""));
	BOOST_CHECK(m.matches("b", "has", ""));
}

BOOST_AUTO_TEST_CASE(NotHasMatcher)
{
	matchers::NotHasMatcher m;

	BOOST_CHECK(m.supports("has_not"));
	BOOST_CHECK(!m.matches("a", "has_not", ""));
	BOOST_CHECK(!m.matches("b", "has_not", ""));
}

BOOST_AUTO_TEST_CASE(ContainsMatcher)
{
	matchers::ContainsMatcher m;

	BOOST_CHECK(m.supports("contains"));
	BOOST_CHECK(m.matches("apa", "contains", "pa"));
	BOOST_CHECK(!m.matches("apa", "contains", "fisk"));
}

BOOST_AUTO_TEST_CASE(NotContainsMatcher)
{
	matchers::NotContainsMatcher m;

	BOOST_CHECK(m.supports("contains_not"));
	BOOST_CHECK(!m.matches("apa", "contains_not", "pa"));
	BOOST_CHECK(m.matches("apa", "contains_not", "fisk"));
}

BOOST_AUTO_TEST_SUITE_END() // matchers_tests

BOOST_AUTO_TEST_SUITE_END() // requirements

BOOST_AUTO_TEST_SUITE_END() // farm
