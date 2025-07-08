file(GLOB_RECURSE TEST_SOURCES src/*.cpp)

add_executable(run_core_tests
    ${TEST_SOURCES}
)

target_link_libraries(run_core_tests PRIVATE
    GTest::gtest_main

    enkas-core
)

include(GoogleTest)
gtest_discover_tests(run_core_tests)
