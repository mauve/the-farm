/*
 * Copyright (C) 2012, All rights reserved, Mikael Olenfalk <mikael@olenfalk.se>
 */

#ifndef UTIL_CONSOLE_CONSOLE_HPP_
#define UTIL_CONSOLE_CONSOLE_HPP_

#include <iosfwd>
#include <vector>

#include <boost/noncopyable.hpp>
#include <boost/smart_ptr/shared_ptr.hpp>

#include <util/console/mode.hpp>

namespace util {

namespace console {

class mode;

class console :
	public boost::noncopyable
{
public:
	console ();
	virtual ~console ();

	// no unique_ptr in C++03 have to live with
	// shared_ptr for now
	void enter_mode (boost::shared_ptr<mode> m);

	virtual std::ostream& out ();
	virtual std::ostream& err ();
	virtual std::istream& in ();

	void quit (int exit_code);

	int run ();

private:
	friend class mode;
	void leave_mode (mode& m);

private:
	bool _quit;
	int _exit_code;
	std::vector<boost::shared_ptr<mode> > _modes;
};

}  // namespace console

}  // namespace util

#endif /* UTIL_CONSOLE_CONSOLE_HPP_ */
