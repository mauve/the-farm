/*
 * Copyright (C) 2012, All rights reserved, Mikael Olenfalk <mikael@olenfalk.se>
 */

#include <iostream>
#include <cmath>
#include <stdexcept>

#include <boost/lexical_cast.hpp>

#include <util/console/console.hpp>

class TestMode :
	public util::console::mode
{
public:
	bool handle_command (const std::string& cmd,
			const std::vector<std::string>& args)
	{
		if (cmd == "help")
		{
			out() << "Show some help" << std::endl;
			return true;
		}
		else if (cmd == "sin")
		{
			if (args.size() != 1)
				throw std::invalid_argument("missing args");

			out() << std::sin(boost::lexical_cast<double>(args[0])) << std::endl;
			return true;
		}
		else if (cmd == "quit")
		{
			con().quit(0);
			return true;
		}

		return false;
	}
};

int main (int argc, char* argv[])
{
	util::console::console console;

	TestMode mode;
	util::console::mode::registration reg = console.enter_mode(mode);

	console.run();

	return 0;
}
