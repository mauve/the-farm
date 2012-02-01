
#ifndef UTIL_PROCESS_CHILD_OPTIONS_HPP_
#define UTIL_PROCESS_CHILD_OPTIONS_HPP_

#include <vector>
#include <map>
#include <string>

#include "stream_id.hpp"

namespace util {

namespace process {

class child_options
{
public:
	enum stream_behaviour {
		inherit,
		pipe_read,
		pipe_write,
		close
	};

	child_options (const std::string& command);
	// TODO: add later
	// child_options (const std::string& commandline);
	~child_options ();

	child_options& add_arg (const std::string& arg);
	child_options& set_args (const std::vector<std::string>& args);

	child_options& use_env (bool use);

	child_options& add_env (const std::string& key, const std::string& value);
	child_options& set_env (const std::map<std::string, std::string>& env);

	child_options& set_default_stream_behaviour (stream_behaviour);
	child_options& set_stream_behaviour (stream_id stream_id, stream_behaviour);

	const std::string& get_command () const;
	const std::vector<std::string>& get_args () const;
	const std::map<std::string, std::string>& get_env () const;
	const std::vector<std::string> get_formatted_env () const;
	const std::map<stream_id, stream_behaviour>& get_stream_behaviours () const;

private:
	std::string _command;
	std::vector<std::string> _args;
	bool _use_env;
	std::map<std::string, std::string> _env;
	std::map<stream_id, stream_behaviour> _streams;
};

}  // namespace process

}  // namespace util

#endif /* UTIL_PROCESS_CHILD_OPTIONS_HPP_ */
