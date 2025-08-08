# --- Source files for tests ---
file(GLOB_RECURSE APP_TEST_SOURCES CONFIGURE_DEPENDS *.cpp)
qt_add_executable(run_app_tests ${APP_TEST_SOURCES})

target_include_directories(run_app_tests PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# --- Include directories ---
target_link_libraries(run_app_tests PRIVATE
    GTest::gmock
    Qt6::Test
    enkas-app
)

# --- C++ Standard ---
target_compile_features(run_app_tests PRIVATE cxx_std_23)

# --- Discover tests ---
include(GoogleTest)
gtest_discover_tests(run_app_tests)
