
#ifndef FARM_AGENT_JOB_INSTANCE_HPP_
#define FARM_AGENT_JOB_INSTANCE_HPP_

#include <boost/smart_ptr/shared_ptr.hpp>

namespace farm {

class JobOrder;

class JobInstance
{
public:
	JobInstance (boost::shared_ptr<JobOrder> order);
	~JobInstance ();


};

}  // namespace farm

#endif /* FARM_AGENT_JOB_INSTANCE_HPP_ */
