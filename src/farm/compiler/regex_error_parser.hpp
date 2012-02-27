/*
 * Copyright (C) 2012, All rights reserved, Mikael Olenfalk <mikael@olenfalk.se>
 */

#ifndef FARM_COMPILER_REGEX_ERROR_PARSER_HPP_
#define FARM_COMPILER_REGEX_ERROR_PARSER_HPP_

#include <farm/compiler/error_info.hpp>
#include <farm/compiler/error_parser.hpp>

#include <boost/regex_fwd.hpp>

namespace farm {

class RegexErrorParser :
	public ErrorParser
{
public:
	struct ParseSetting
	{
		ParseSetting (ErrorInfo::level_t _level,
				const std::string& _regex,
				int _file,
				int _line = -1,
				int _column = -1,
				int _description = -1);
		~ParseSetting ();

		ErrorInfo::level_t level;
		std::string regex;
		int file; // -1 means ignore
		int line; // -1 means ignore
		int column; // -1 means ignore
		int description; // -1 means ignore
	};

	RegexErrorParser (
			const std::vector<ParseSetting>& patterns,
			const std::vector<std::string>& ignore_patterns);
	virtual ~RegexErrorParser ();

	virtual void parse_line (const std::string& str);

	virtual const std::vector<ErrorInfo>& parsed_errors ();

private:
	typedef	std::vector<boost::regex> ignore_parsers_t;
	ignore_parsers_t _ignore_parsers;
	typedef std::vector<std::pair<ParseSetting, boost::regex> > parsers_t;
	parsers_t _parsers;
	std::vector<ErrorInfo> _parsed_errors;
};

}  // namespace farm

#endif /* FARM_COMPILER_REGEX_ERROR_PARSER_HPP_ */
