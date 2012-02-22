
#ifndef FARM_COMPILER_RUNNER_CONFIG_HPP_
#define FARM_COMPILER_RUNNER_CONFIG_HPP_

#include <string>
#include <vector>

namespace farm {

class ErrorParser;

class CompilerRunnerConfig
{
public:
	virtual ~CompilerRunnerConfig () = 0;

	virtual std::string get_path () = 0;
	virtual std::vector<std::string> get_arguments (const std::string& code_path,
													const std::string& target_path) = 0;

	virtual ErrorParser& get_error_parser () = 0;
};

}  // namespace farm

#endif /* FARM_COMPILER_RUNNER_CONFIG_HPP_ */
