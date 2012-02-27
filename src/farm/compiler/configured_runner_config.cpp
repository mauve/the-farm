/*
 * Copyright (C) 2012, All rights reserved, Mikael Olenfalk <mikael@olenfalk.se>
 */

#include <farm/compiler/configured_runner_config.hpp>
#include <farm/compiler/error_parser.hpp>

#include <util/string/expand.hpp>

#include <stdexcept>

namespace farm {

ConfiguredCompilerRunnerConfig::ConfiguredCompilerRunnerConfig(const std::string& path,
																const std::vector<std::string>& argument_templates,
																boost::shared_ptr<ErrorParser> error_parser)
	: _path(path),
	  _argument_templates(argument_templates),
	  _error_parser(error_parser)
{
	if (!error_parser)
		throw std::invalid_argument("error_parser cannot be null");
}

ConfiguredCompilerRunnerConfig::~ConfiguredCompilerRunnerConfig()
{}

std::string ConfiguredCompilerRunnerConfig::get_path ()
{
	return _path;
}

std::vector<std::string> ConfiguredCompilerRunnerConfig::get_arguments (const std::string& code_path,
																		const std::string& target_path)
{
	std::vector<std::string> arguments;
	std::map<std::string, std::string> replacements;
	replacements["code-path"] = code_path;
	replacements["target-path"] = target_path;

	for (std::vector<std::string>::const_iterator iter = _argument_templates.begin();
			iter != _argument_templates.end(); ++iter)
	{
		arguments.push_back(util::expand_copy(*iter, replacements));
	}

	return arguments;
}

ErrorParser& ConfiguredCompilerRunnerConfig::get_error_parser()
{
	return *_error_parser;
}

}  // namespace farm
