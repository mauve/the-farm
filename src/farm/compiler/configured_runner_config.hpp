
#ifndef FARM_COMPILER_CONFIGURED_RUNNER_CONFIG_HPP_
#define FARM_COMPILER_CONFIGURED_RUNNER_CONFIG_HPP_

#include <farm/compiler/runner_config.hpp>
#include <boost/smart_ptr/shared_ptr.hpp>

namespace farm {

class ConfiguredCompilerRunnerConfig :
	public CompilerRunnerConfig
{
public:
	ConfiguredCompilerRunnerConfig (const std::string& path,
									const std::vector<std::string>& argument_templates,
									boost::shared_ptr<ErrorParser> error_parser);
	~ConfiguredCompilerRunnerConfig ();

	std::string get_path ();
	std::vector<std::string> get_arguments (const std::string& code_path,
											const std::string& target_path);

	ErrorParser& get_error_parser ();

private:
	std::string _path;
	std::vector<std::string> _argument_templates;
	boost::shared_ptr<ErrorParser> _error_parser;
};

}  // namespace farm

#endif /* FARM_COMPILER_CONFIGURED_RUNNER_CONFIG_HPP_ */
