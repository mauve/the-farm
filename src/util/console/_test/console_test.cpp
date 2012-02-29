/*
 * Copyright (C) 2012, All rights reserved, Mikael Olenfalk <mikael@olenfalk.se>
 */

#include <iostream>
#include <cmath>
#include <stdexcept>

#include <boost/lexical_cast.hpp>
#include <boost/smart_ptr/make_shared.hpp>

#include <util/console/console.hpp>
#include <util/console/auto_mode.hpp>

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

class TestMode2 :
	public util::console::auto_mode<TestMode2>
{
public:
	TestMode2 ()
		: util::console::auto_mode<TestMode2>(this, std::string("2> "))
	{
		register_function("add", &TestMode2::add);
		register_function("sub", &TestMode2::sub);
	}

private:
	int add(int a, int b)
	{
		return a + b;
	}

	int sub(int a, int b)
	{
		return a - b;
	}
};

int main (int argc, char* argv[])
{
	util::console::console console;

	console.enter_mode(boost::make_shared<TestMode>());

	console.run();

	console.enter_mode(boost::make_shared<TestMode2>());

	console.run();

	return 0;
}
