# --- Source files for tests ---
file(GLOB_RECURSE CORE_TEST_SOURCES CONFIGURE_DEPENDS *.cpp)

add_executable(run_core_tests ${CORE_TEST_SOURCES})

# --- Include directories ---
target_link_libraries(run_core_tests PRIVATE
    GTest::gtest_main
    enkas-core
)

# --- C++ Standard ---
target_compile_features(run_core_tests PRIVATE cxx_std_23)

# --- Discover tests ---
include(GoogleTest)
gtest_discover_tests(run_core_tests)
