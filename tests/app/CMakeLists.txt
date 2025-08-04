# --- Source files for tests ---
file(GLOB_RECURSE APP_TEST_SOURCES "*.cpp")

qt_add_executable(run_app_tests
    ${APP_TEST_SOURCES}
)

target_include_directories(run_app_tests PRIVATE
    ${PROJECT_SOURCE_DIR}/app/src
)

# --- Include directories ---
target_link_libraries(run_app_tests PRIVATE
    GTest::gtest_main
    enkas-app
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Test
)

# --- C++ Standard ---
target_compile_features(run_app_tests PRIVATE cxx_std_23)

# --- Discover tests ---
include(GoogleTest)
gtest_discover_tests(run_app_tests)