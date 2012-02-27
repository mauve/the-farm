/*
 * Copyright (C) 2012, All rights reserved, Mikael Olenfalk <mikael@olenfalk.se>
 */

#include <boost/test/unit_test.hpp>

#include <farm/requirements/matcher_registry.hpp>
#include <farm/requirements/matchers.hpp>
#include <farm/requirements/requirement.hpp>

BOOST_AUTO_TEST_SUITE(farm)

BOOST_AUTO_TEST_SUITE(requirements)

BOOST_AUTO_TEST_SUITE(matcher_registry_tests)

BOOST_AUTO_TEST_CASE(empty)
{
	MatcherRegistry reg;

	BOOST_CHECK(!reg.has_matcher("=="));
	BOOST_CHECK(!reg.matches("a", "==", "a"));
}

BOOST_AUTO_TEST_CASE(full)
{
	MatcherRegistry reg;

	reg.register_matcher(new matchers::EqualMatcher);
	reg.register_matcher(new matchers::NotEqualMatcher);

	BOOST_CHECK(reg.has_matcher("=="));
	BOOST_CHECK(reg.has_matcher("!="));
	BOOST_CHECK(!reg.has_matcher("has"));

	BOOST_CHECK(reg.matches("a", "==", "a"));
	BOOST_CHECK(reg.matches("a", "!=", "b"));
}

BOOST_AUTO_TEST_CASE(multi_match)
{
	MatcherRegistry reg;

	reg.register_matcher(new matchers::ContainsMatcher);
	reg.register_matcher(new matchers::EqualMatcher);
	reg.register_matcher(new matchers::GreaterEqualMatcher);
	reg.register_matcher(new matchers::GreaterMatcher);
	reg.register_matcher(new matchers::HasMatcher);
	reg.register_matcher(new matchers::LessEqualMatcher);
	reg.register_matcher(new matchers::LessMatcher);
	reg.register_matcher(new matchers::NotContainsMatcher);
	reg.register_matcher(new matchers::NotEqualMatcher);
	reg.register_matcher(new matchers::NotHasMatcher);

	std::vector<Requirement> reqs;
	reqs.push_back(Requirement("compiler", "==", "gcc"));
	reqs.push_back(Requirement("version", ">", "123"));
	reqs.push_back(Requirement("version", "<", "200"));

	std::map<std::string, std::string> props;
	props["compiler"] = "gcc";
	props["version"] = "150";

	std::vector<Requirement> failed_reqs = reg.matches(reqs, props);

	BOOST_CHECK(failed_reqs.empty());
	if (!failed_reqs.empty()) {
		for (std::vector<Requirement>::const_iterator iter = failed_reqs.begin();
				iter != failed_reqs.end(); ++iter)
		{
			BOOST_MESSAGE("failed req: " << *iter);
		}
	}
}

BOOST_AUTO_TEST_SUITE_END() // matcher_registry_tests

BOOST_AUTO_TEST_SUITE_END() // requirements

BOOST_AUTO_TEST_SUITE_END() // farm
