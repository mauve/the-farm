/*
 * Copyright (C) 2012, All rights reserved, Mikael Olenfalk <mikael@olenfalk.se>
 */

#ifndef FARM_AGENT_CONTROLLER_HPP_
#define FARM_AGENT_CONTROLLER_HPP_

#include <farm/agent/agent.hpp>
#include <farm/compiler/requirement.hpp>

#include <vector>
#include <set>

#include <boost/thread/mutex.hpp>
#include <boost/smart_ptr/shared_ptr.hpp>

namespace farm {

class AgentController
{
public:
	AgentController ();
	~AgentController ();

	// TODO: use RAII for this
	void register_agent (const AgentInfo& info);
	void unregister_agent (const AgentInfo& info);

	std::vector<AgentInfo> all_agents ();

	std::vector<AgentInfo> find_compatible_agents (const CompilerRequirement& req);

private:
	boost::mutex _mutex;
	typedef std::set<AgentInfo> agents_t;
	agents_t _known_agents;
};

}  // namespace farm

#endif /* FARM_AGENT_CONTROLLER_HPP_ */
