
#include <util/console/console.hpp>

#include <iostream>
#include <boost/tokenizer.hpp>

namespace util {

namespace console {

console::console ()
{}

console::~console()
{
	while (!_modes.empty())
	{
		mode* m = _modes.top();
		m->detach();
	}
}

mode::registration console::enter_mode (mode& m)
{
	_modes.push(&m);
	return m.attach(*this);
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
		mode& current_mode = *_modes.top();
		std::cout << current_mode.prompt();
		std::string input;
		if (std::getline (std::cin, input))
		{
			typedef boost::escaped_list_separator<char> separator_t;
			typedef boost::tokenizer<separator_t> tokenizer_t;
			tokenizer_t tok(input, separator_t('\\', ' ', '"'));
			tokenizer_t::iterator begin = tok.begin(), end = tok.end();

			if (begin == end)
				continue;

			std::string command = *begin++;
			std::vector<std::string> args(begin, end);

			// TODO: check if handled
			try {
				if (!current_mode.handle_command(command, args))
					std::cerr << "Error: Unknown Command: '" << command << "'" << std::endl;
			} catch (const std::exception& ex) {
				std::cerr << "Exception: " << ex.what() << std::endl;
			}

			std::cout << std::flush;
			std::cerr << std::flush;
		}
	}

	return _exit_code;
}

void console::leave_mode (mode& m)
{
	if (_modes.top() == &m) {
		_modes.pop();
	}
}

}  // namespace console

}  // namespace util
