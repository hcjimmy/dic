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

# Build library containing core logic and it's tests.
add_subdirectory(libdie)

add_library(flagi flagi/flagi.c)

target_link_libraries(dic PRIVATE die flagi)

install(TARGETS dic)


