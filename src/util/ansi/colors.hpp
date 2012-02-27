/*
 * Copyright (C) 2012, All rights reserved, Mikael Olenfalk <mikael@olenfalk.se>
 */

#ifndef UTIL_ANSI_COLORS_HPP_
#define UTIL_ANSI_COLORS_HPP_

#include <iostream>
#include <locale>

namespace util {

namespace ansi {

enum color_t {
	black, red, green, yellow, blue, magenta,
	cyan, white, ignore, default_setting
};

class color_setting;

class color_facet :
	public std::locale::facet
{
public:
	static const std::locale::id id;

	color_facet ();
	color_facet (bool enabled);

	void enable (bool enabled);
	bool is_enabled () const;

	void apply_reset (std::ostream& os) const;
	void apply_color (std::ostream& os, const color_setting& set) const;

private:
	bool _is_enabled;
};

class color_setting
{
public:
	color_setting (color_t fg, color_t bg = ignore);

	color_t foreground() const;
	color_t background() const;

private:
	color_t _fg, _bg;
};

std::ostream& reset_all (std::ostream& os);

std::ostream& operator<< (std::ostream& os, const color_setting& set);

/*
 * black, red, green, yellow, blue, magenta,
 * cyan, white, ignore, default_setting
 */
extern const color_setting fg_black, fg_red, fg_green,
	fg_yellow, fg_blue, fg_magenta, fg_cyan, fg_white,
	fg_default_setting, bg_black, bg_red, bg_green,
	bg_yellow, bg_blue, bg_magenta, bg_cyan, bg_white,
	bg_default_setting;

}  // namespace ansi

}  // namespace util

#endif /* UTIL_ANSI_COLORS_HPP_ */
