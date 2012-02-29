/*
 * Copyright (C) 2012, All rights reserved, Mikael Olenfalk <mikael@olenfalk.se>
 */

#ifndef UTIL_CONSOLE_MODE_HPP_
#define UTIL_CONSOLE_MODE_HPP_

#include <vector>
#include <string>
#include <iosfwd>

#include <boost/smart_ptr/shared_ptr.hpp>

namespace util {

namespace console {

class console;

class mode
{
public:
	friend class console; // for attach, detach

	mode (const std::string& __default_prompt = "> ");
	virtual ~mode ();

	virtual std::string prompt ();

	virtual bool handle_command (const std::string& cmd,
			const std::vector<std::string>& args) = 0;

protected:
	virtual void before_detach ();
	virtual void after_attach ();

	void detach ();

	// throws if unattached
	console& con ();

	std::ostream& out ();
	std::ostream& err ();
	std::istream& in ();

private:
	void attach (console& con);

	std::string _default_prompt;
	std::ostream _null_out, _null_err;
	std::istream _null_in;
	console* _con;
};

}  // namespace console

}  // namespace util

#endif /* UTIL_CONSOLE_MODE_HPP_ */
