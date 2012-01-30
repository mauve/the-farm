/*
 * Copyright (C) 2012, All rights reserved, Mikael Olenfalk <mikael@olenfalk.se>
 */

#include <farm/agent/agent.hpp>

namespace farm {

const boost::uuids::uuid& AgentInfo::id () const
{
	return _id;
}

const boost::posix_time::ptime& AgentInfo::last_seen () const
{
	return _last_seen;
}

const std::vector<CompilerDescription>& AgentInfo::supported_compilers () const
{
	return _supported_compilers;
}

bool operator< (const AgentInfo& l, const AgentInfo& r)
{
	return l.id() < r.id();
}

}  // namespace farm


