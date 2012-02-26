
#ifndef UTIL_IOSTREAM_READFILE_HPP_
#define UTIL_IOSTREAM_READFILE_HPP_

#include <ios>
#include <vector>
#include <string>

namespace util {

bool readfile (const std::string& path,
		std::vector<char>& buf,
		std::size_t max_size = 0,
		std::ios::openmode mode = std::ios::binary | std::ios::in);

bool readfile (std::ifstream& ifs, std::vector<char>& buf,
		std::size_t max_size = 0);

}  // namespace util

#endif /* UTIL_IOSTREAM_READFILE_HPP_ */
