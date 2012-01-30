/*
 * Copyright (C) 2012, All rights reserved, Mikael Olenfalk <mikael@olenfalk.se>
 */

#ifndef FARM_JOB_ORDER_HPP_
#define FARM_JOB_ORDER_HPP_

#include <string>
#include <vector>

#include <boost/uuid/uuid.hpp>

namespace farm {

class CompilerRequirement;

class JobOrder
{
public:
	JobOrder (const boost::uuids::uuid& id,
				const std::vector<CompilerRequirement>& compilers,
				const std::string& source_code,
				const std::string& input);

	~JobOrder ();

	const boost::uuids::uuid& get_id () const;

	const std::vector<CompilerRequirement>& required_compilers () const;

	const std::string& code () const;
	const std::string& input () const;

private:
	boost::uuids::uuid _id;
	std::vector<CompilerRequirement> _required_compilers;

	std::string _code;
	std::string _input;
};

}  // namespace farm

#endif /* FARM_JOB_ORDER_HPP_ */
