cmake_minimum_required(VERSION 3.20)
project(dic LANGUAGES C VERSION 0.1)

include(CTest)

# Default to enable link-time-optimizations if supported.
include(CheckIPOSupported)
check_ipo_supported(RESULT lto_supported)
if (lto_supported)
	set(CMAKE_INTERPROCEDURAL_OPTIMIZATION true)
endif()


# Main executable
add_executable(dic
	main.c print_dierrors.c)

# The core logic is in this library.
add_library(die	libdie/libdie.c
	libdie/parse_exp.c
	libdie/parse_operation.c
	libdie/string_ops.c
	libdie/list_defs.c
	libdie/libdie.c
	)
# Add math to die.
target_link_libraries(die m)

add_library(flagi flagi/flagi.c)

target_link_libraries(dic PRIVATE die flagi)

install(TARGETS dic)

# Testing

add_executable(tests
	main.test.c
	libdie/tests/libdie.test.c
	libdie/tests/string_ops.test.c
	libdie/list/tests/list.test.c)

target_link_libraries(tests PRIVATE die)

add_test(NAME tests COMMAND tests)
