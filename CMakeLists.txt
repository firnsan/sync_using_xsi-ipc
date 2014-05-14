cmake_minimum_required(VERSION 2.6)

project(sync)

set(CMAKE_BUILD_TYPE "Debug")

set(CMAKE_CXX_COMPILER "gcc")
set(CMAKE_CXX_FLAGS_DEBUG "-O0")
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)


add_subdirectory(mutil-console)
add_subdirectory(one-console)