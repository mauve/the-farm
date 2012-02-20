

#ifndef FARM_REQUIREMENTS_REQUIREMENT_HPP_
#define FARM_REQUIREMENTS_REQUIREMENT_HPP_

#include <string>
#include <iosfwd>

namespace farm {

namespace requirements {

class Requirement
{
public:
	Requirement (const std::string& name,
					const std::string& op = "==",
					const std::string& match = "");
	~Requirement ();

	const std::string& get_name () const;
	const std::string& get_op () const;
	const std::string& get_match () const;

private:
	std::string _name;
	std::string _op;
	std::string _match;
};

std::ostream& operator<< (std::ostream& os, const Requirement& r);

}  // namespace requirements

}  // namespace farm


#endif /* FARM_REQUIREMENTS_REQUIREMENT_HPP_ */
