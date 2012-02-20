/*
 * Copyright (C) 2012, All rights reserved, Mikael Olenfalk <mikael@olenfalk.se>
 */

#include <farm/compiler/requirement.hpp>
#include <farm/version.hpp>

#include <algorithm>

namespace farm {

using std::string;
using std::vector;
using std::pair;

CompilerRequirement::CompilerRequirement()
	: _null_requirement(true)
{}

CompilerRequirement::CompilerRequirement(const string& name, const Version& specver)
	: _null_requirement(false),
	  _name(name),
	  _version_range(specver, specver)
{}

CompilerRequirement::CompilerRequirement(const string& name, const Version& low, const Version& high)
	: _null_requirement(false),
	  _name(name),
	  _version_range(low, high)
{}

CompilerRequirement::~CompilerRequirement()
{}

bool CompilerRequirement::is_null_requirement() const
{
	return _null_requirement;
}

void CompilerRequirement::set_required_compiler(const string& name)
{
	_name = name;
}

const string& CompilerRequirement::get_name () const
{
	return _name;
}

bool CompilerRequirement::requires_version() const
{
	return _version_range.first != Version::null_version &&
			_version_range.second != Version::null_version;
}

void CompilerRequirement::set_specific_version(const Version& version)
{
	_version_range.first = version;
	_version_range.second = version;
}

void CompilerRequirement::set_version_range(const Version& low, const Version& high)
{
	_version_range.first = low;
	_version_range.second = high;
}

const std::pair<Version, Version>& CompilerRequirement::get_version_range() const
{
	return _version_range;
}

}  // namespace farm
