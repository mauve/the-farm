
#ifndef FARM_ERROR_INFO_HPP_
#define FARM_ERROR_INFO_HPP_

#include <string>
#include <iosfwd>

namespace farm {

class ErrorInfo
{
public:
	enum level_t {
		error, warning, notice
	};

	ErrorInfo (level_t lvl, const std::string& message);
	ErrorInfo (level_t lvl, const std::string& file,
			int row, int column, const std::string& message);
	~ErrorInfo ();

	bool has_position () const;

	level_t get_level () const;

	const std::string& get_file () const;

	int get_line () const;
	int get_column () const;

	const std::string& get_message () const;

private:
	level_t _level;
	std::string _file;
	int _line, _column;
	std::string _message;
};

std::ostream& operator<< (std::ostream& os, ErrorInfo::level_t lvl);
std::ostream& operator<< (std::ostream& os, const ErrorInfo& err);

}  // namespace farm

#endif /* FARM_ERROR_INFO_HPP_ */
