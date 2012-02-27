/*
 * Copyright (C) 2012, All rights reserved, Mikael Olenfalk <mikael@olenfalk.se>
 */

#ifndef FARM_JOB_RESULT_HPP_
#define FARM_JOB_RESULT_HPP_

#include <vector>
#include <map>

#include <boost/uuid/uuid.hpp>

#include <farm/compiler/description.hpp>

namespace farm {

class ErrorInfo;
class CompilerDescription;

namespace jobs {

class JobResult
{
public:
	enum result_data_t {
		compiler_standard_output,
		compiler_standard_error,
		binary,
		runtime_standard_output,
		runtime_standard_error
	};

	JobResult (const boost::uuids::uuid& agent_id,
				const CompilerDescription& compiler,
				const std::vector<ErrorInfo>& messages);
	~JobResult ();

	const boost::uuids::uuid& agent_id () const;
	const CompilerDescription& compiler_description () const;

	bool succeeded () const;

	const std::vector<ErrorInfo>& messages () const;

	std::vector<char>& output (result_data_t res);
	const std::vector<char>& output (result_data_t res) const;

private:
	boost::uuids::uuid _agent_id;
	CompilerDescription _compiler_description;
	std::vector<ErrorInfo> _messages;

	std::map<result_data_t, std::vector<char> > _outputs;
};

}  // namespace jobs

}  // namespace farm

#endif /* FARM_JOB_RESULT_HPP_ */
