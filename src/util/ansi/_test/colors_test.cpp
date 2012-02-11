
#include <iostream>
#include <util/ansi/colors.hpp>

using namespace util::ansi;

int main (int argc, char* argv[])
{
	std::locale loc(std::locale(), new color_facet(true));
	std::cout.imbue(loc);

	const char* color_names[] = {
		"black", "red", "green", "yellow", "blue",
		"magenta", "cyan", "ignore", "default_setting"
	};
	int num_color_names = sizeof(color_names)/sizeof(color_names[0]);

	color_t colors[] = {
		black, red, green, yellow, blue, magenta,
		cyan, white, ignore, default_setting
	};

	for (int i = 0; i < num_color_names; ++i) {
		for (int j = 0; j < num_color_names; ++j) {
			std::cout << color_setting(colors[i], colors[j]);
			std::cout << "foreground = " << color_names[i] << " ";
			std::cout << "background = " << color_names[j] << "\n";
			std::cout << reset_all;
		}
	}

	return 0;
}
