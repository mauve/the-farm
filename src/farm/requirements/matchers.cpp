/*
 * Copyright (C) 2012, All rights reserved, Mikael Olenfalk <mikael@olenfalk.se>
 */

#include <farm/requirements/matchers.hpp>

namespace farm {

namespace requirements {

namespace matchers {

SingleOpMatcher::SingleOpMatcher (const std::string& op)
	: _op(op)
{}

SingleOpMatcher::~SingleOpMatcher ()
{}

bool SingleOpMatcher::supports (const std::string& op) const
{
	return _op == op;
}

#define DECLARE_SIMPLE_MATCHER(name, op) \
	name::name () \
		: SingleOpMatcher(#op) \
	{} \
	\
	name::~name () \
	{} \
	\
	bool name::matches (const std::string& left, \
						const std::string&, \
						const std::string& right) const \
	{ \
		return left op right; \
	}

DECLARE_SIMPLE_MATCHER(EqualMatcher, ==);
DECLARE_SIMPLE_MATCHER(NotEqualMatcher, !=);
DECLARE_SIMPLE_MATCHER(LessMatcher, <);
DECLARE_SIMPLE_MATCHER(GreaterMatcher, >);
DECLARE_SIMPLE_MATCHER(LessEqualMatcher, <=);
DECLARE_SIMPLE_MATCHER(GreaterEqualMatcher, >=);

/*
 * Has
 */

HasMatcher::HasMatcher ()
	: SingleOpMatcher("has")
{}

HasMatcher::~HasMatcher ()
{}

bool HasMatcher::matches (const std::string& left,
					const std::string&,
					const std::string& right) const
{
	return true;
}

/*
 * NotHas
 */

NotHasMatcher::NotHasMatcher ()
	: SingleOpMatcher("has_not")
{}

NotHasMatcher::~NotHasMatcher ()
{}

bool NotHasMatcher::matches (const std::string& left,
								const std::string&,
								const std::string& right) const
{
	return false;
}

/*
 * Contains
 */
ContainsMatcher::ContainsMatcher()
	: SingleOpMatcher("contains")
{}

ContainsMatcher::~ContainsMatcher()
{}

bool ContainsMatcher::matches (const std::string& left,
								const std::string& op,
								const std::string& right) const
{
	return left.find(right) != std::string::npos;
}

/*
 * NotContains
 */
NotContainsMatcher::NotContainsMatcher()
	: SingleOpMatcher("contains_not")
{}

NotContainsMatcher::~NotContainsMatcher()
{}

bool NotContainsMatcher::matches (const std::string& left,
								const std::string& op,
								const std::string& right) const
{
	return left.find(right) == std::string::npos;
}

}  // namespace matchers

}  // namespace requirements

}  // namespace famr
