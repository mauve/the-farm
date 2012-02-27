/*
 * Copyright (C) 2012, All rights reserved, Mikael Olenfalk <mikael@olenfalk.se>
 */

#ifndef UTIL_PROCESS_STREAMDESCRIPTOR_HPP_
#define UTIL_PROCESS_STREAMDESCRIPTOR_HPP_

#include <boost/config.hpp>

#include <boost/asio/windows/stream_handle.hpp>
#include <boost/asio/posix/stream_descriptor.hpp>

namespace util {

namespace process {

#ifdef _WIN32
typedef boost::asio::windows::stream_handle stream_descriptor;
#else
typedef boost::asio::posix::stream_descriptor stream_descriptor;
#endif

}  // namespace process

}  // namespace util

#endif /* UTIL_PROCESS_STREAMDESCRIPTOR_HPP_ */
