/*
 * Copyright (C) 2012, All rights reserved, Mikael Olenfalk <mikael@olenfalk.se>
 */

#ifndef UTIL_PROCESS__DETAIL_SPAWNER_HPP_
#define UTIL_PROCESS__DETAIL_SPAWNER_HPP_

#include <map>

#include <boost/system/error_code.hpp>

#include <util/process/native_process_id.hpp>
#include <util/process/stream_descriptor.hpp>

namespace util {

namespace process {

class stream_id;
class child_options;

namespace _detail {

native_process_id spawn (boost::asio::io_service& io_service,
		const child_options& opt,
		std::map<stream_id, boost::shared_ptr<stream_descriptor> >& streams,
		boost::system::error_code& ec);

}  // namespace _detail

}  // namespace process

}  // namespace util


#endif /* UTIL_PROCESS__DETAIL_SPAWNER_HPP_ */
