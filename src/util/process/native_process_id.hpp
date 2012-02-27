/*
 * Copyright (C) 2012, All rights reserved, Mikael Olenfalk <mikael@olenfalk.se>
 */

#ifndef UTIL_PROCESS_NATIVE_PROCESS_ID_HPP_
#define UTIL_PROCESS_NATIVE_PROCESS_ID_HPP_

#ifdef _WIN32
#	define WIN32_LEAN_AND_MEAN
#	include <windows.h>
#else
#	include <sys/types.h>
#endif

namespace util {

namespace process {

#ifdef _WIN32
typedef HANDLE native_process_id;
#else
typedef pid_t native_process_id;
#endif

}  // namespace process

}  // namespace util

#endif /* UTIL_PROCESS_NATIVE_PROCESS_ID_HPP_ */
