/*
 * Copyright (C) 2012, All rights reserved, Mikael Olenfalk <mikael@olenfalk.se>
 */

#ifndef FARM_REQUIREMENTS_MATCHER_HPP_
#define FARM_REQUIREMENTS_MATCHER_HPP_

#include <string>
#include <map>

namespace farm {

namespace requirements {

class Matcher
{
public:
	virtual ~Matcher () = 0;

	virtual bool supports (const std::string& op) const = 0;

	virtual bool matches (const std::string& left,
							const std::string& op,
							const std::string& right) const = 0;
};

}  // namespace requirements

}  // namespace farm

#endif /* FARM_REQUIREMENTS_MATCHER_HPP_ */
