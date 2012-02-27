/*
 * Copyright (C) 2012, All rights reserved, Mikael Olenfalk <mikael@olenfalk.se>
 */

#include <farm/compiler/regex_error_parser.hpp>

#include <boost/regex.hpp>
#include <boost/lexical_cast.hpp>

namespace farm {

/*
 * RegexErrorParser::ParseSetting
 */
RegexErrorParser::ParseSetting::ParseSetting (
		ErrorInfo::level_t _level,
		const std::string& _regex,
		int _file,
		int _line /* = -1 */,
		int _column /* = -1 */,
		int _description /* = -1 */)
	: level(_level),
	  regex(_regex),
	  file(_file),
	  line(_line),
	  column(_column),
	  description(_description)
{}

RegexErrorParser::ParseSetting::~ParseSetting ()
{}

/*
 * RegexErrorParser
 */

RegexErrorParser::RegexErrorParser (const std::vector<ParseSetting>& patterns,
									const std::vector<std::string>& ignore_patterns)
{
	{
		_ignore_parsers.reserve(ignore_patterns.size());
		for (std::vector<std::string>::const_iterator iter = ignore_patterns.begin();
				iter != ignore_patterns.end(); ++iter)
		{
			_ignore_parsers.push_back(boost::regex(*iter));
		}
	}

	{
		_parsers.reserve(patterns.size());
		for (std::vector<ParseSetting>::const_iterator iter = patterns.begin();
				iter != patterns.end(); ++iter)
		{
			_parsers.push_back(std::make_pair(*iter, boost::regex(iter->regex)));
		}
	}
}

RegexErrorParser::~RegexErrorParser ()
{}

void RegexErrorParser::parse_line (const std::string& line)
{
	for (ignore_parsers_t::const_iterator iter = _ignore_parsers.begin();
			iter != _ignore_parsers.end(); ++iter)
	{
		if (boost::regex_match(line, *iter))
			return;
	}

	for (parsers_t::const_iterator iter = _parsers.begin();
			iter != _parsers.end(); ++iter)
	{
		boost::smatch m;
		if (boost::regex_match(line, m, iter->second)) {
			std::string message;
			std::string file;
			int line = -1, column = -1;
			try {
				if (iter->first.file != -1 && m[iter->first.file].matched)
					file = m[iter->first.file];
				if (iter->first.description != -1 && m[iter->first.description].matched)
					message = m[iter->first.description];
				if (iter->first.line != -1 && m[iter->first.line].length())
					line = boost::lexical_cast<int>(m[iter->first.line]);
				if (iter->first.column != -1 && m[iter->first.column].length())
					column = boost::lexical_cast<int>(m[iter->first.column]);
			} catch (...) {}

			ErrorInfo error(iter->first.level, file, line, column, message);
			_parsed_errors.push_back(error);
		}
	}
}

const std::vector<ErrorInfo>& RegexErrorParser::parsed_errors ()
{
	return _parsed_errors;
}

}  // namespace farm
