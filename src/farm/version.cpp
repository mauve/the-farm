/*
 * Copyright (C) 2012, All rights reserved, Mikael Olenfalk <mikael@olenfalk.se>
 */

#include "version.hpp"

namespace farm {

const Version Version::null_version;

Version::Version ()
	: major(0), minor(0), micro(0), build(0)
{}

Version::Version (int _major, int _minor, int _micro, int _build)
	: major(_major), minor(_minor), micro(_micro), build(_build)
{}

Version::~Version ()
{}

bool operator== (const Version& l, const Version& r)
{
	return l.major == r.major && l.minor == r.minor &&
			l.micro == r.micro && l.build == r.build;
}

bool operator!= (const Version& l, const Version& r)
{
	return !(l == r);
}

bool operator< (const Version& l, const Version& r)
{
	return l.major < r.major && l.minor < r.minor &&
			l.micro < r.micro && l.build < r.build;
}

bool operator<= (const Version& l, const Version& r)
{
	return l.major <= r.major && l.minor <= r.minor &&
			l.micro <= r.micro && l.build <= r.build;
}

}  // namespace farm

