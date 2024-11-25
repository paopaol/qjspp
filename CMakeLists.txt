cmake_minimum_required(VERSION 3.15)

project(qjs++)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

add_subdirectory(tests)
add_subdirectory(benchmark)
