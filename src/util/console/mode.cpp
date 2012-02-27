/*
 * Copyright (C) 2012, All rights reserved, Mikael Olenfalk <mikael@olenfalk.se>
 */

#include <util/console/mode.hpp>
#include <util/console/console.hpp>

#include <stdexcept>

namespace util {

namespace console {

mode::mode (const std::string& default_prompt)
	: _default_prompt(default_prompt),
	  _out(&std::cout),
	  _err(&std::cerr),
	  _in(&std::cin),
	  _con(0)
{
}

mode::~mode ()
{
	detach ();
}

std::string mode::prompt ()
{
	return _default_prompt;
}

mode::registration mode::attach (console& con)
{
	detach ();
	_con = &con;

	return registration(*this);
}

console& mode::con ()
{
	if (!_con)
		std::runtime_error("mode is not attached");

	return *_con;
}

std::ostream& mode::out ()
{
	return *_out;
}

std::ostream& mode::err ()
{
	return *_err;
}

std::istream& mode::in ()
{
	return *_in;
}

void mode::detach ()
{
	if (!_con)
		return;

	_con->leave_mode(*this);
	_con = 0;
}

/*
 * registration
 */

struct mode::registration::impl
{
	impl (mode& m)
		: _mode(&m)
	{}

	~impl ()
	{
		detach ();
	}

	void detach ()
	{
		if (_mode)
			_mode->detach ();
	}

	mode* _mode;
};

mode::registration::registration ()
{}

mode::registration::registration (mode& m)
	: _pimpl(new impl(m))
{}

mode::registration::~registration ()
{}

void mode::registration::detach ()
{
	if (_pimpl)
		_pimpl->detach ();
	_pimpl.reset ();
}

}  // namespace console

}  // namespace util
