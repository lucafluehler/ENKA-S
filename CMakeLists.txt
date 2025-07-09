cmake_minimum_required(VERSION 3.24)
project(Enkas VERSION 2.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 6.2 REQUIRED COMPONENTS
    Widgets
    OpenGLWidgets
    Gui
    Charts
)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.17.0.zip
  DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE) # For Windows MSVC
FetchContent_MakeAvailable(googletest)

add_subdirectory(core)
add_subdirectory(app)

option(ENKAS_BUILD_TESTS "Build the unit tests" ON)
if(ENKAS_BUILD_TESTS)
    enable_testing() # Enables CTest support
    add_subdirectory(tests)
endif()
