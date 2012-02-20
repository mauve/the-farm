/*
 * Copyright (C) 2012, All rights reserved, Mikael Olenfalk <mikael@olenfalk.se>
 */

#ifndef FARM_JOB_ORDER_HPP_
#define FARM_JOB_ORDER_HPP_

#include <string>
#include <vector>

#include <boost/uuid/uuid.hpp>

namespace farm {

namespace requirements {

class Requirement;

}  // namespace requirements

namespace jobs {

class JobOrder
{
public:
	JobOrder (const boost::uuids::uuid& id);
	JobOrder (const boost::uuids::uuid& id,
				const std::vector<requirements::Requirement>& requirements,
				const std::string& source_code,
				const std::string& input);
	~JobOrder ();

	const boost::uuids::uuid& get_id () const;

	std::vector<requirements::Requirement>& get_requirements ();
	const std::vector<requirements::Requirement>& get_requirements () const;

	const std::string& get_code () const;
	const std::string& get_input () const;

private:
	boost::uuids::uuid _id;
	std::vector<requirements::Requirement> _requirements;

	std::string _code;
	std::string _input;
};

}  // namespace jobs

}  // namespace farm

#endif /* FARM_JOB_ORDER_HPP_ */
