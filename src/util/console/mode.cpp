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
	  _null_out(0), _null_err(0),
	  _null_in(0), _con(0)
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

void mode::attach (console& con)
{
	detach ();
	_con = &con;

	after_attach();
}

console& mode::con ()
{
	if (!_con)
		std::runtime_error("mode is not attached");

	return *_con;
}

std::ostream& mode::out ()
{
	if (_con)
		return _con->out();
	return _null_out;
}

std::ostream& mode::err ()
{
	if (_con)
		return _con->err();
	return _null_err;
}

std::istream& mode::in ()
{
	if (_con)
		return _con->in();
	return _null_in;
}

void mode::detach ()
{
	if (!_con)
		return;

	before_detach();
	_con->leave_mode(*this);
	_con = 0;
}

void mode::before_detach ()
{
	/* default impl does nothing */
}

void mode::after_attach ()
{
	/* default impl does nothing */
}

}  // namespace console

}  // namespace util
