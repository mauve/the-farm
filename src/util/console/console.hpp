/*
 * Copyright (C) 2012, All rights reserved, Mikael Olenfalk <mikael@olenfalk.se>
 */

#ifndef UTIL_CONSOLE_CONSOLE_HPP_
#define UTIL_CONSOLE_CONSOLE_HPP_

#include <iosfwd>
#include <stack>

#include <boost/noncopyable.hpp>

#include <util/console/mode.hpp>

namespace util {

namespace console {

class mode;

class console :
	public boost::noncopyable
{
public:
	console ();
	~console ();

	mode::registration enter_mode (mode& m);

	void quit (int exit_code);

	int run ();

private:
	friend class mode;
	void leave_mode (mode& m);

	// does not pop
	void _leave_mode (mode& m);

private:
	bool _quit;
	int _exit_code;
	std::stack<mode*> _modes;
};

}  // namespace console

}  // namespace util

#endif /* UTIL_CONSOLE_CONSOLE_HPP_ */
