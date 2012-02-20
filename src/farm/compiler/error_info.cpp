
#include <farm/compiler/error_info.hpp>

#include <iostream>

namespace farm {

ErrorInfo::ErrorInfo(level_t lvl, const std::string& message)
	: _level(lvl), _row(-1), _column(-1), _message(message)
{}

ErrorInfo::ErrorInfo(level_t lvl, int row, int column,
						const std::string& message)
	: _level(lvl), _row(row), _column(column),
	  _message(message)
{}

ErrorInfo::~ErrorInfo ()
{}

bool ErrorInfo::has_position () const
{
	return _row >= 0 && _column >= 0;
}

ErrorInfo::level_t ErrorInfo::get_level () const
{
	return _level;
}

int ErrorInfo::get_row () const
{
	return _row;
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
	if (err.has_position()) {
		os << err.get_row() << ":" << err.get_column() << ": ";
	}
	os << err.get_message();
	return os;
}

}  // namespace farm


