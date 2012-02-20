/*
 * Copyright (C) 2012, All rights reserved, Mikael Olenfalk <mikael@olenfalk.se>
 */

#ifndef FARM_COMPILER_DESCRIPTION_HPP_
#define FARM_COMPILER_DESCRIPTION_HPP_

#include <string>
#include <vector>

#include <farm/version.hpp>

namespace farm {

struct CompilerRequirement;

class CompilerDescription
{
public:
	CompilerDescription(const std::string& name, const std::string& version_info,
						const Version& version);
	~CompilerDescription ();

	const std::string& get_name () const;
	const std::string& get_version_info () const;
	const Version& get_version () const;

	bool supports (const CompilerRequirement& req) const;

private:
	std::string _name;
	std::string _version_info;
	Version _version;
};

}  // namespace farm

#endif /* FARM_COMPILER_DESCRIPTION_HPP_ */
