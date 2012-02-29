/*
 * Copyright (C) 2012, All rights reserved, Mikael Olenfalk <mikael@olenfalk.se>
 */

#include <util/console/console.hpp>

#include <iostream>
#include <boost/tokenizer.hpp>
#include <boost/bind.hpp>

namespace util {

namespace console {

console::console ()
{}

console::~console()
{
	std::for_each(_modes.rend(), _modes.rbegin(),
			boost::bind(&mode::detach, _1));
}

void console::enter_mode (boost::shared_ptr<mode> m)
{
	_modes.push_back(m);
	m->attach(*this);
}

std::ostream& console::out ()
{
	return std::cout;
}

std::ostream& console::err ()
{
	return std::cerr;
}

std::istream& console::in ()
{
	return std::cin;
}

void console::quit (int exit_code)
{
	_quit = true;
	_exit_code = exit_code;
}

int console::run ()
{
	_exit_code = 0;
	_quit = false;
	while (!_quit && !_modes.empty())
	{
		mode& current_mode = *_modes.back();
		out() << current_mode.prompt();
		std::string input;
		if (std::getline (in(), input))
		{
			typedef boost::escaped_list_separator<char> separator_t;
			typedef boost::tokenizer<separator_t> tokenizer_t;
			tokenizer_t tok(input, separator_t('\\', ' ', '"'));
			tokenizer_t::iterator begin = tok.begin(), end = tok.end();

			if (begin == end)
				continue;

			std::string command = *begin++;
			std::vector<std::string> args(begin, end);

			try {
				if (!current_mode.handle_command(command, args))
					err() << "Error: Unknown Command: '" << command << "'" << std::endl;
			} catch (const std::exception& ex) {
				err() << "Exception: " << ex.what() << std::endl;
			}

			out() << std::flush;
			err() << std::flush;
		}
	}

	return _exit_code;
}

void console::leave_mode (mode& m)
{
	for (std::vector<boost::shared_ptr<mode> >::iterator iter = _modes.begin();
			iter != _modes.end(); ++iter)
	{
		if ((*iter).get() == &m) {
			_modes.erase(iter);
			return;
		}
	}
}

}  // namespace console

}  // namespace util
