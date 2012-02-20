
#include <farm/requirements/requirement.hpp>
#include <iostream>

namespace farm {

namespace requirements {

using std::string;

Requirement::Requirement (const string& name,
							const string& op /* = "==" */,
							const string& match /* = "" */)
	: _name(name), _op(op), _match(match)
{}

Requirement::~Requirement ()
{}

const string& Requirement::get_name () const
{
	return _name;
}

const string& Requirement::get_op () const
{
	return _op;
}

const string& Requirement::get_match () const
{
	return _match;
}

std::ostream& operator<< (std::ostream& os, const Requirement& r)
{
	os << r.get_name() << ' ' << r.get_op() << ' ' << r.get_match();
	return os;
}

}  // namespace requirements

}  // namespace farm
