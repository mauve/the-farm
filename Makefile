
package = the-farm

util_HDRS = \
			src/util/functional/member-function-registry.hpp \
			src/util/memstream.hpp \
			src/util/posix/pipe.hpp \
			src/util/process/child.hpp \
			src/util/process/child_options.hpp \
			src/util/process/parent.hpp \
			src/util/process/stream_id.hpp \
			src/util/process/_detail/zero_terminated.hpp

util_SRCS = \
			src/util/memstream.cpp \
			src/util/posix/pipe.cpp \
			src/util/process/child.cpp \
			src/util/process/child_options.cpp \
			src/util/process/parent.cpp \
			src/util/process/stream_id.cpp \
			src/util/process/_detail/zero_terminated.cpp

PROGRAMS = \
			util-test \
			child-process \
			process-test

INCLUDES = src

util-test_SRCS = ${util_SRCS} \
				 src/util/util_test.cpp \
				 src/util/memstream_tests.cpp \
				 src/util/process/_detail/zero_terminated_tests.cpp

util-test_LIBS = boost_system boost_thread boost_unit_test_framework

child-process_SRCS = \
				src/util/process/_test/child_process.cpp

process-test_SRCS = ${util_SRCS} \
				src/util/process/_test/process_tests.cpp

process-test_LIBS = boost_system boost_thread
process-test_INCLUDES = src
process-test_DEFINES = CHILD_PROCESS_PATH="\"`pwd`/bin/Debug/child-process\""

include common.mk