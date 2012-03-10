/*
 * Copyright (C) 2012, All rights reserved, Mikael Olenfalk <mikael@olenfalk.se>
 */

#include <util/iostream/readfile.hpp>

#include <fstream>

namespace util {

bool readfile (const std::string& path,
		std::vector<char>& buf,
		std::size_t max_size /* = 0 */,
		std::ios::openmode mode /* = std::ios::binary | std::ios::in */)
{
	std::ifstream ifs(path.c_str(), mode);
	if (!ifs.is_open())
		return false;

	return readfile(ifs, buf, max_size);
}

bool readfile (std::ifstream& ifs, std::vector<char>& buf,
		std::size_t max_size /* = 0 */)
{
	ifs.seekg(0, std::ifstream::end);
	std::size_t size = ifs.tellg();
	ifs.seekg(0, std::ifstream::beg);

	if (max_size && (max_size > size))
		size = max_size;

	if (size) {
		buf.resize(size);
		if (! ifs.read(&buf[0], size))
			return false;
	}

	return true;
}

}  // namespace util
