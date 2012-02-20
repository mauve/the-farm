/*
 * Copyright (C) 2012, All rights reserved, Mikael Olenfalk <mikael@olenfalk.se>
 */

#ifndef FARM_COMPILER_REQUIREMENT_HPP_
#define FARM_COMPILER_REQUIREMENT_HPP_

#include <string>
#include <vector>

#include <farm/version.hpp>

namespace farm {

class CompilerRequirement
{
public:
	CompilerRequirement ();
	CompilerRequirement (const std::string& _name, const Version& specific_version);
	CompilerRequirement (const std::string& _name, const Version& low, const Version& high);
	~CompilerRequirement ();

	bool is_null_requirement () const;

	void set_required_compiler (const std::string& name);
	const std::string& get_name () const;

	bool requires_version () const;
	void set_specific_version (const Version& version);
	void set_version_range (const Version& low, const Version& high);

	const std::pair<Version, Version>& get_version_range () const;

private:
	bool _null_requirement;
	std::string _name;
	std::pair<Version, Version> _version_range;
	std::string _standard;
};

}  // namespace farm

#endif /* FARM_COMPILER_REQUIREMENT_HPP_ */
