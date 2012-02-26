
#ifndef SPAWNER_POSIX_SPAWN_HPP_
#define SPAWNER_POSIX_SPAWN_HPP_

#ifndef __UTIL_PROCESS__DETAIL_SPAWNER_CPP__INCLUDING
#	error You may not include this file yourself.
#endif

#include <spawn.h>

#include <algorithm> // for_each

#include <boost/smart_ptr/shared_ptr.hpp>

#include <util/posix/pipe.hpp>
#include <util/process/_detail/zero_teminated.hpp>

namespace util {

namespace process {

namespace _detail {

pid_t spawn (boost::asio::io_service& io_service,
		const child_options& opt,
		std::map<stream_id, boost::shared_ptr<stream_descriptor> >& streams,
		boost::system::error_code& ec)
{
	typedef	std::map<stream_id, child_options::stream_behaviour> beh_t;

	posix_spawn_file_actions_t file_actions;
	posix_spawn_file_actions_init(&file_actions);

	streams.clear();

	std::vector<int> close_later;

	const beh_t& behaviours = opt.get_stream_behaviours();
	for (beh_t::const_iterator iter = behaviours.begin(); iter != behaviours.end(); ++iter)
	{
		switch (iter->second)
		{
			case child_options::close:
			{
				posix_spawn_file_actions_addclose(&file_actions, iter->first.native());
				break;
			}

			case child_options::inherit:
			{
				posix_spawn_file_actions_addinherit_np(&file_actions, iter->first.native());
				break;
			}

			case child_options::pipe_read:
			{
				posix::pipe p;
				close_later.push_back(p.read_end());
				posix_spawn_file_actions_adddup2(&file_actions,
						p.steal_read_end(),
						iter->first.native());
				streams.insert(
							std::make_pair(
								iter->first,
								boost::shared_ptr<stream_descriptor>(
									new stream_descriptor(io_service, p.steal_write_end()))
							)
						);
				break;
			}

			case child_options::pipe_write:
			{
				posix::pipe p;
				close_later.push_back(p.write_end());
				posix_spawn_file_actions_adddup2(&file_actions,
						p.steal_write_end(),
						iter->first.native());
				streams.insert(
							std::make_pair(
								iter->first,
								boost::shared_ptr<stream_descriptor>(
									new stream_descriptor(io_service, p.steal_read_end()))
							)
						);
				break;
			}
		}
	}

	zero_terminated_list_of_strings argv;
	argv.add(opt.get_command());
	argv.add(opt.get_args());

	pid_t child_pid = -1;

	const char* _path = opt.get_command().c_str();
	char* const* _argv = argv.data();
	int result = posix_spawn(&child_pid,
							 _path,
							 &file_actions,
							 NULL,
							 _argv,
							 NULL);

	posix_spawn_file_actions_destroy(&file_actions);

	std::for_each(close_later.begin(), close_later.end(), ::close);

	ec.assign(result, boost::system::generic_category());
	return child_pid;
}

}  // namespace _detail

}  // namespace process

}  // namespace util

#endif /* SPAWNER_POSIX_SPAWN_HPP_ */
