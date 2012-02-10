
/*
 * this program is used by the util/process unit-tests
 * to test that the process subsystem works as expected
 */

#include <iostream>
#include <sstream>
#include <string>

#include <unistd.h>

int main (int argc, char* argv[])
{
	if (argc == 1) {
		std::cout << "Hello World!" << std::endl;
		usleep(1000000);
		return 0;
	}

	int mode;
	std::istringstream is(argv[1]);
	if (!(is >> mode)) {
		std::cout << "Could not convert argv[1] (=" << argv[1] << ") to int." << std::endl;
		return 0;
	}

	switch (mode)
	{
		default:
			std::cout << "Invalid mode (" << mode << ")" << std::endl;
			return 0;

		case 1: {
			std::cerr << "Hello World!" << std::endl;
			return 0;
		}

		case 2: {
			std::cerr << "Hello World!" << std::endl;
			return 1;
		}

		case 3: {
			std::string line;
			if (!std::getline(std::cin, line)) {
				std::cout << "No data on std::cin" << std::endl;
				return 1;
			}

			std::cout << line << std::endl;
			std::cerr << line << std::endl;
			return 0;
		}

		case 4: {
			std::cerr << "Hello Err World!" << std::endl;
			std::cout << "Hello Out World!" << std::endl;
			return 0;
		}

		case 5: {
			return 2;
		}
	}

	return 0;
}
