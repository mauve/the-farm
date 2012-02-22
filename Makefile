
package = the-farm

util_HDRS = \
			src/util/functional/member-function-registry.hpp \
			src/util/memstream.hpp \
			src/util/ansi/colors.hpp \
			src/util/buffer/appendable_buffer.hpp \
			src/util/iostream/imemstream.hpp \
			src/util/posix/pipe.hpp \
			src/util/process/child.hpp \
			src/util/process/child_options.hpp \
			src/util/process/parent.hpp \
			src/util/process/stream_id.hpp \
			src/util/process/_detail/zero_terminated.hpp \
			src/util/string/expand.hpp

util_SRCS = \
			src/util/memstream.cpp \
			src/util/ansi/colors.cpp \
			src/util/buffer/appendable_buffer.cpp \
			src/util/iostream/imemstream.cpp \
			src/util/posix/pipe.cpp \
			src/util/process/child.cpp \
			src/util/process/child_options.cpp \
			src/util/process/parent.cpp \
			src/util/process/stream_id.cpp \
			src/util/process/_detail/zero_terminated.cpp \
			src/util/string/expand.cpp

farm_base_HDRS = \
			${util_HDRS} \
			src/farm/agent/agent.hpp \
			src/farm/agent/controller.hpp \
			src/farm/agent/job_instance.hpp \
			src/farm/compiler/description.hpp \
			src/farm/compiler/error_info.hpp \
			src/farm/compiler/error_parser.hpp \
			src/farm/compiler/regex_error_parser.hpp \
			src/farm/compiler/requirement.hpp \
			src/farm/compiler/runner.hpp \
			src/farm/jobs/order.hpp \
			src/farm/jobs/result.hpp \
			src/farm/requirements/matcher.hpp \
			src/farm/requirements/matcher_registry.hpp \
			src/farm/requirements/matchers.hpp \
			src/farm/requirements/requirement.hpp \
			src/farm/version.hpp

farm_base_SRCS = \
			${util_SRCS} \
			src/farm/agent/agent.cpp \
			src/farm/agent/controller.cpp \
			src/farm/compiler/configured_runner_config.cpp \
			src/farm/compiler/description.cpp \
			src/farm/compiler/error_info.cpp \
			src/farm/compiler/error_parser.cpp \
			src/farm/compiler/regex_error_parser.cpp \
			src/farm/compiler/requirement.cpp \
			src/farm/compiler/runner_config.cpp \
			src/farm/compiler/runner.cpp \
			src/farm/jobs/order.cpp \
			src/farm/jobs/result.cpp \
			src/farm/requirements/matcher.cpp \
			src/farm/requirements/matcher_registry.cpp \
			src/farm/requirements/matchers.cpp \
			src/farm/requirements/requirement.cpp \
			src/farm/version.cpp

farm_test_SRCS = \
			src/farm/requirements/matcher_registry_tests.cpp \
			src/farm/requirements/matchers_tests.cpp \
			src/farm/compiler/configured_runner_config_tests.cpp \
			src/farm/compiler/regex_error_parser_tests.cpp

PROGRAMS = \
			util-test \
			child-process \
			process-test \
			colors-test \
			farm-test \
			farm

INCLUDES = src

util-test_SRCS = ${util_SRCS} \
				 src/util/util_test.cpp \
				 src/util/memstream_tests.cpp \
				 src/util/ansi/colors_tests.cpp \
				 src/util/buffer/appendable_buffer_tests.cpp \
				 src/util/iostream/imemstream_tests.cpp \
				 src/util/process/_detail/zero_terminated_tests.cpp \
				 src/util/string/expand_tests.cpp

util-test_LIBS = boost_system boost_thread boost_unit_test_framework boost_regex

child-process_SRCS = \
				src/util/process/_test/child_process.cpp

colors-test_SRCS = \
				src/util/ansi/colors.cpp \
				src/util/ansi/_test/colors_test.cpp

process-test_SRCS = ${util_SRCS} \
				src/util/process/_test/process_tests.cpp

process-test_LIBS = boost_system boost_thread boost_regex
process-test_INCLUDES = src
process-test_DEFINES = CHILD_PROCESS_PATH="\"`pwd`/bin/Debug/child-process\""

farm_base_LIBS = boost_system \
					boost_thread \
					boost_regex \
					boost_program_options \
					boost_filesystem \
					boost_regex

farm-test_SRCS = \
			${farm_base_SRCS} \
			${farm_test_SRCS} \
			src/farm/farm_test.cpp

farm-test_LIBS = ${farm_base_LIBS} boost_unit_test_framework

farm_SRCS = \
			${farm_base_SRCS} \
			main.cpp

farm_LIBS = ${farm_base_LIBS}

include common.mk