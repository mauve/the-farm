/*
 * Copyright (C) 2012, All rights reserved, Mikael Olenfalk <mikael@olenfalk.se>
 */

#ifndef FARM_COMPILER_ERROR_PARSER_HPP_
#define FARM_COMPILER_ERROR_PARSER_HPP_

#include <string>
#include <vector>

namespace farm {

class ErrorInfo;

class ErrorParser
{
public:
	virtual ~ErrorParser () = 0;

	virtual void parse_line (const std::string& str) = 0;

	virtual const std::vector<ErrorInfo>& parsed_errors () = 0;
};

}  // namespace farm

#endif /* FARM_COMPILER_ERROR_PARSER_HPP_ */
