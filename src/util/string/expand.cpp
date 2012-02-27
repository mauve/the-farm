/*
 * Copyright (C) 2012, All rights reserved, Mikael Olenfalk <mikael@olenfalk.se>
 */

#include <util/string/expand.hpp>
#include <boost/regex.hpp>

#include <iostream>

namespace util {

typedef std::map<std::string, std::string> map_t;

namespace {

struct replacer :
	public std::unary_function<const boost::smatch&, const std::string&>
{
public:
	replacer (const map_t& replacements)
		: _replacements(replacements)
	{}

	const std::string& operator() (const boost::smatch& match) const
	{
		map_t::const_iterator iter = _replacements.find(match[1]);
		if (iter == _replacements.end())
			return _empty;

		return iter->second;
	}

private:
	const map_t& _replacements;
	std::string _empty;
};

}  // namespace

std::string expand_copy (const std::string& haystack, const map_t& replacements)
{
	boost::regex rx("\\$\\{([^}]+)\\}");
	replacer replacer_func(replacements);
	return boost::regex_replace(haystack, rx, replacer_func);
}

}  // namespace util
