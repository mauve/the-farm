/*
 * Copyright (C) 2012, All rights reserved, Mikael Olenfalk <mikael@olenfalk.se>
 */

#ifndef FARM_COMPILER_HPP_
#define FARM_COMPILER_HPP_

#include <string>
#include <vector>

#include <farm/version.hpp>

namespace farm {

struct CompilerRequirement;
// struct UnfulfilledRequirement;

class CompilerDescription
{
public:
	CompilerDescription(const std::string& name, const std::string& version_info,
						const Version& version, const std::vector<std::string>& supported_standards);
	~CompilerDescription ();

	const std::string& get_name () const;
	const std::string& get_version_info () const;
	const Version& get_version () const;
	const std::vector<std::string>& get_supported_standards () const;

	bool supports (const CompilerRequirement& req) const;

private:
	std::string _name;
	std::string _version_info;
	Version _version;
	std::vector<std::string> _supported_standards;

	// TODO: implement later
	// bool supports (const CompilerRequirement& req, UnfulfilledRequirement& missing);
};

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

#endif /* FARM_COMPILER_HPP_ */
