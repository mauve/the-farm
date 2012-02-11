
#include "colors.hpp"

#include <fstream>

namespace util {

namespace ansi {

const std::locale::id color_facet::id;

color_facet::color_facet ()
	: _is_enabled(false)
{}

color_facet::color_facet (bool enabled)
	: _is_enabled(enabled)
{}

void color_facet::enable (bool enabled)
{
	_is_enabled = enabled;
}

bool color_facet::is_enabled () const
{
	return _is_enabled;
}

void color_facet::apply_reset (std::ostream& os) const
{
	if (!_is_enabled)
		return;

	std::ostream::sentry cerberus(os);
	os << "\x1b[0;0m";
}

void color_facet::apply_color (std::ostream& os, const color_setting& set) const
{
	if (!_is_enabled || (set.foreground() == ignore && set.background() == ignore))
		return;

	std::ostream::sentry cerberus(os);
	os << "\x1b[";
	color_t fg = set.foreground();
	color_t bg = set.background();
	switch (fg) {
	case ignore:
		break;
	default:
		os << (30 + (int)fg);
	}

	switch(bg) {
	case ignore:
		break;
	default:
		if (fg != ignore)
			os << ';';
		os << (40 + (int)bg);
	}

	os << "m";
}

/*
 * color_setting
 */

color_setting::color_setting (color_t fg, color_t bg /* = ignore */)
	: _fg(fg), _bg(bg)
{}

color_t color_setting::foreground() const
{
	return _fg;
}

color_t color_setting::background() const
{
	return _bg;
}

std::ostream& reset_all (std::ostream& os)
{
	std::locale loc = os.getloc();
	if (std::has_facet<color_facet>(loc)) {
		const color_facet& cf = std::use_facet<color_facet>(loc);
		cf.apply_reset(os);
	}
	return os;
}

std::ostream& operator<< (std::ostream& os, const color_setting& set)
{
	std::locale loc = os.getloc();
	if (std::has_facet<color_facet>(loc)) {
		const color_facet& cf = std::use_facet<color_facet>(loc);
		cf.apply_color(os, set);
	}
	return os;
}

/*
 * predefined colors
 */
const color_setting fg_black(black), fg_red(red),
		fg_green(green), fg_yellow(yellow), fg_blue(blue),
		fg_magenta(magenta), fg_cyan(cyan), fg_white(white),
		fg_default_setting(default_setting),
		bg_black(ignore, black), bg_red(ignore, red),
		bg_green(ignore, green), bg_yellow(ignore, yellow),
		bg_blue(ignore, blue), bg_magenta(ignore, magenta),
		bg_cyan(ignore, cyan), bg_white(ignore, white),
		bg_default_setting(ignore, default_setting);

}  // namespace ansi

}  // namespace util


