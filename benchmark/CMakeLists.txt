find_package(benchmark)

add_executable(qjs++benchmark)
target_sources(qjs++benchmark PRIVATE FunctionBenckmark.cc)
target_link_libraries(qjs++benchmark PRIVATE benchmark::benchmark
                                             benchmark::benchmark_main)
target_include_directories(qjs++benchmark PRIVATE ${PROJECT_SOURCE_DIR}/include)
target_include_directories(qjs++benchmark
                           PRIVATE ${PROJECT_SOURCE_DIR}/usr/include)
target_link_directories(qjs++benchmark PRIVATE
                        ${PROJECT_SOURCE_DIR}/usr/lib/quickjs)
target_link_libraries(qjs++benchmark PRIVATE quickjs)
