
/*
 * simple trick to remove the need for conditional
 */

#include <util/process/child_options.hpp>
#include <util/process/stream_id.hpp>
#include <util/process/stream_descriptor.hpp>

#define __UTIL_PROCESS__DETAIL_SPAWNER_CPP__INCLUDING 1
#ifdef _WIN32
#	include "spawner_win32_create_process.hpp"
#else
#	include "spawner_posix_spawn.hpp"
#endif
