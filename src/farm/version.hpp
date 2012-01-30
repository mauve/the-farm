/*
 * Copyright (C) 2012, All rights reserved, Mikael Olenfalk <mikael@olenfalk.se>
 */

#ifndef FARM_VERSION_HPP_
#define FARM_VERSION_HPP_

namespace farm {

class Version
{
public:
	Version ();
	Version (int major, int minor, int micro, int build);
	~Version ();

	int major;
	int minor;
	int micro;
	int build;

	const static Version null_version;
};

bool operator== (const Version& left, const Version& right);
bool operator!= (const Version& left, const Version& right);
bool operator<  (const Version& left, const Version& right);
bool operator<= (const Version& left, const Version& right);

}  // namespace farm


#endif /* FARM_VERSION_HPP_ */
