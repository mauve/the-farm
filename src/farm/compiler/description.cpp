/*
 * Copyright (C) 2012, All rights reserved, Mikael Olenfalk <mikael@olenfalk.se>
 */

#include <farm/compiler/description.hpp>
#include <farm/compiler/requirement.hpp>
#include <farm/version.hpp>

#include <algorithm>

namespace farm {

using std::string;
using std::vector;
using std::pair;

/*
 * CompilerDescription
 */

CompilerDescription::CompilerDescription (const string& name, const string& version_info,
											const Version& version)
	: _name(name), _version_info(version_info), _version(version)
{
}

CompilerDescription::~CompilerDescription()
{}

const string& CompilerDescription::get_name () const
{
	return _name;
}

const string& CompilerDescription::get_version_info () const
{
	return _version_info;
}

const Version& CompilerDescription::get_version () const
{
	return _version;
}

bool CompilerDescription::supports (const CompilerRequirement& req) const
{
	if (req.is_null_requirement())
		return true;

	const string& required_name = req.get_name();

	if (!required_name.empty()) {
		if (required_name != _name)
			return false;
	}

	const pair<Version, Version>& required_version_range = req.get_version_range ();
	if (required_version_range.second != Version::null_version) {
		if (!(required_version_range.first < _version &&
				_version < required_version_range.second))
			return false;
	}

	return true;
}

}  // namespace farm
