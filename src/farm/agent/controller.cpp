/*
 * Copyright (C) 2012, All rights reserved, Mikael Olenfalk <mikael@olenfalk.se>
 */

#include <farm/agent/controller.hpp>
#include <farm/compiler/description.hpp>

#include <boost/thread/locks.hpp>

namespace farm {

using std::vector;
using std::multimap;
using std::string;

AgentController::AgentController ()
{}

AgentController::~AgentController ()
{}

void AgentController::register_agent(const AgentInfo& info)
{
	boost::unique_lock<boost::mutex> lock(_mutex);
	_known_agents.insert(info);
}

void AgentController::unregister_agent(const AgentInfo& info)
{
	boost::unique_lock<boost::mutex> lock(_mutex);
	_known_agents.erase(info);
}

vector<AgentInfo> AgentController::all_agents()
{
	boost::unique_lock<boost::mutex> lock(_mutex);
	return vector<AgentInfo>(_known_agents.begin(), _known_agents.end());
}

vector<AgentInfo> AgentController::find_compatible_agents(const CompilerRequirement& req)
{
	if (req.is_null_requirement())
		return all_agents();

	boost::unique_lock<boost::mutex> lock(_mutex);

	// TODO: performance suck
	vector<AgentInfo> agents;
	for (agents_t::const_iterator iter = _known_agents.begin();
			iter != _known_agents.end(); ++iter)
	{
		const vector<CompilerDescription>& descs = iter->supported_compilers();
		for (vector<CompilerDescription>::const_iterator comp_iter = descs.begin();
					comp_iter != descs.end(); ++comp_iter)
		{
			if (comp_iter->supports(req))
			{
				agents.push_back(*iter);
				break;
			}
		}
	}

	return agents;
}


}  // namespace farm



