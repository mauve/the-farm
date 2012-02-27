/*
 * Copyright (C) 2012, All rights reserved, Mikael Olenfalk <mikael@olenfalk.se>
 */

#ifndef FARM_REQUIREMENTS_MATCHER_REGISTRY_HPP_
#define FARM_REQUIREMENTS_MATCHER_REGISTRY_HPP_

#include <string>
#include <vector>
#include <map>

namespace farm {

namespace requirements {

class Matcher;
class Requirement;

class MatcherRegistry
{
public:
	MatcherRegistry ();
	~MatcherRegistry ();

	// takes ownership
	void register_matcher (Matcher* m);

	bool has_matcher (const std::string& op) const;

	bool matches (const std::string& left,
					const std::string& op,
					const std::string& right) const;

	std::vector<Requirement> matches (const std::vector<Requirement>& reqs,
										const std::map<std::string, std::string>& set) const;

private:
	std::vector<Matcher*>::const_iterator find_matcher (const std::string& op) const;

	std::vector<Matcher*> _matchers;
};

}  // namespace requirements

}  // namespace farm

#endif /* FARM_REQUIREMENTS_MATCHER_REGISTRY_HPP_ */
