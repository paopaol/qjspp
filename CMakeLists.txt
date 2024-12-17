cmake_minimum_required(VERSION 3.15)

project(qjs++)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)


set(CMAKE_PREFIX_PATH ${CMAKE_SOURCE_DIR}/usr/lib64/cmake/quickjs)


find_package(qjs REQUIRED)

add_subdirectory(tests)
add_subdirectory(benchmark)
