/*
 * Copyright (C) 2012, All rights reserved, Mikael Olenfalk <mikael@olenfalk.se>
 */

#ifndef FARM_REQUIREMENTS_MATCHERS_HPP_
#define FARM_REQUIREMENTS_MATCHERS_HPP_

#include <farm/requirements/matcher.hpp>

namespace farm {

namespace requirements {

namespace matchers {

class SingleOpMatcher :
	public Matcher
{
public:
	SingleOpMatcher (const std::string& op);
	~SingleOpMatcher ();

	bool supports (const std::string& op) const;

private:
	std::string _op;
};

#define DEFINE_SIMPLE_MATCHER(name) \
	class name : \
		public SingleOpMatcher \
	{ \
	public: \
		name (); \
		~name (); \
		\
		bool matches (const std::string& left, \
						const std::string& op, \
						const std::string& right) const; \
	}

DEFINE_SIMPLE_MATCHER(EqualMatcher);
DEFINE_SIMPLE_MATCHER(NotEqualMatcher);
DEFINE_SIMPLE_MATCHER(LessMatcher);
DEFINE_SIMPLE_MATCHER(GreaterMatcher);
DEFINE_SIMPLE_MATCHER(LessEqualMatcher);
DEFINE_SIMPLE_MATCHER(GreaterEqualMatcher);
DEFINE_SIMPLE_MATCHER(HasMatcher);
DEFINE_SIMPLE_MATCHER(NotHasMatcher);
DEFINE_SIMPLE_MATCHER(ContainsMatcher);
DEFINE_SIMPLE_MATCHER(NotContainsMatcher);

}  // namespace matchers

}  // namespace requirements

}  // namespace farm

#endif /* FARM_REQUIREMENTS_MATCHERS_HPP_ */
