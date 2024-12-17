find_package(GTest)

add_executable(qjs++test)
target_sources(qjs++test PRIVATE ValueTraitsTest.cc RuntimeTest.cc
                                 ContextTest.cc ValueTest.cc)
target_link_libraries(qjs++test PRIVATE GTest::gtest GTest::gtest_main)
target_include_directories(qjs++test PRIVATE ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(qjs++test PRIVATE qjs dl)
