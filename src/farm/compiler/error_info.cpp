/*
 * Copyright (C) 2012, All rights reserved, Mikael Olenfalk <mikael@olenfalk.se>
 */

#include <farm/compiler/error_info.hpp>

#include <iostream>

namespace farm {

ErrorInfo::ErrorInfo(level_t lvl, const std::string& message)
	: _level(lvl), _line(-1), _column(-1), _message(message)
{}

ErrorInfo::ErrorInfo(level_t lvl,
						const std::string& file,
						int row, int column,
						const std::string& message)
	: _level(lvl), _file(file), _line(row), _column(column),
	  _message(message)
{}

ErrorInfo::~ErrorInfo ()
{}

bool ErrorInfo::has_position () const
{
	return _line >= 0 && _column >= 0;
}

ErrorInfo::level_t ErrorInfo::get_level () const
{
	return _level;
}

const std::string& ErrorInfo::get_file () const
{
	return _file;
}

int ErrorInfo::get_line () const
{
	return _line;
}

int ErrorInfo::get_column () const
{
	return _column;
}

const std::string& ErrorInfo::get_message () const
{
	return _message;
}

std::ostream& operator<< (std::ostream& os, ErrorInfo::level_t lvl)
{
	switch (lvl) {
	default:
	case ErrorInfo::error:
		os << "ERROR"; break;
	case ErrorInfo::warning:
		os << "WARNING"; break;
	case ErrorInfo::notice:
		os << "NOTICE"; break;
	}

	return os;
}

std::ostream& operator<< (std::ostream& os, const ErrorInfo& err)
{
	os << err.get_level() << ": ";
	if (!err.get_file().empty()) {
		os << err.get_file() << ":";
	}
	if (err.get_line() >= 0) {
		os << err.get_line() << ":";
		if (err.get_column() >= 0)
			os << err.get_column() << ":";
	}
	os << " " << err.get_message();
	return os;
}

}  // namespace farm


