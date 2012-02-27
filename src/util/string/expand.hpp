/*
 * Copyright (C) 2012, All rights reserved, Mikael Olenfalk <mikael@olenfalk.se>
 */

#ifndef UTIL_STRING_EXPAND_HPP_
#define UTIL_STRING_EXPAND_HPP_

#include <string>
#include <map>

namespace util {

// replace all occurences of ${string}
// with the result of replacements[string]
// or the empty string if string is not in replacements
std::string expand_copy (const std::string& haystack, const std::map<std::string, std::string>& replacements);

}  // namespace util

#endif /* UTIL_STRING_EXPAND_HPP_ */
