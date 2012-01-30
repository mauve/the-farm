/*
 * Copyright (C) 2012, All rights reserved, Mikael Olenfalk <mikael@olenfalk.se>
 */

#ifndef FARM_AGENT_HPP_
#define FARM_AGENT_HPP_

#include <vector>

#include <boost/uuid/uuid.hpp>
#include <boost/date_time/posix_time/ptime.hpp>

namespace farm {

class CompilerDescription;

class AgentInfo
{
public:
	const boost::uuids::uuid& id () const;
	const boost::posix_time::ptime& last_seen () const;
	const std::vector<CompilerDescription>& supported_compilers () const;

private:
	boost::uuids::uuid _id;
	boost::posix_time::ptime _last_seen;
	std::vector<CompilerDescription> _supported_compilers;
};

bool operator< (const AgentInfo& left, const AgentInfo& right);

}  // namespace farm

#endif /* FARM_AGENT_HPP_ */
