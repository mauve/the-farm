/*
 * mode.hpp
 *
 *  Created on: Feb 27, 2012
 *      Author: olenfalk
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
	class registration;

	mode (const std::string& default_prompt = "> ");
	virtual ~mode ();

	virtual std::string prompt ();

	registration attach (console& con);
	void detach (); // no need to call this yourself

	virtual bool handle_command (const std::string& cmd,
			const std::vector<std::string>& args) = 0;

protected:
	console& con ();

	std::ostream& out ();
	std::ostream& err ();
	std::istream& in ();

private:
	std::string _default_prompt;
	std::ostream* _out,* _err;
	std::istream* _in;
	console* _con;
};

class mode::registration
{
public:
	friend class mode;

	registration ();
	~registration ();

	void detach ();

private:
	registration (mode& m);

	struct impl;
	boost::shared_ptr<impl> _pimpl;
};

}  // namespace console

}  // namespace util

#endif /* UTIL_CONSOLE_MODE_HPP_ */
