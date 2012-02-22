
#ifndef FARM_COMPILER_RUNNER_HPP_
#define FARM_COMPILER_RUNNER_HPP_

#include <vector>

#include <boost/asio/io_service.hpp>
#include <boost/function.hpp>
#include <boost/thread/mutex.hpp>

#include <util/process/parent.hpp>
#include <util/process/stream_id.hpp>
#include <util/buffer/appendable_buffer.hpp>

namespace farm {

namespace jobs {

class JobOrder;
class JobResult;

}  // namespace jobs

class CompilerRunnerConfig;

class CompilerRunner
{
public:
	typedef boost::function<void (boost::shared_ptr<jobs::JobOrder>, boost::shared_ptr<jobs::JobResult>)> job_callback_t;

	CompilerRunner (boost::asio::io_service& io_service);
	~CompilerRunner ();

	// TODO: capability to wait for result
	void run_job (boost::shared_ptr<jobs::JobOrder> order,
					boost::shared_ptr<CompilerRunnerConfig> runner_config,
					const job_callback_t& cb);

private:
	struct running_child_t;
	void _on_read_stream (const boost::system::error_code& ec,
							std::size_t num_bytes_read,
							const util::process::stream_id& sid,
							util::buffer::appendable_buffer& buf,
							boost::shared_ptr<running_child_t>);
	void _on_child_stopped (util::process::parent::child_pointer child,
							boost::shared_ptr<running_child_t>);
	void _queue_read_stream (const util::process::stream_id& sid,
								util::buffer::appendable_buffer& buf,
								boost::shared_ptr<running_child_t>&);

	boost::asio::io_service& _io_service;
	util::process::parent _parent;
};

}  // namespace farm

#endif /* FARM_COMPILER_RUNNER_HPP_ */
