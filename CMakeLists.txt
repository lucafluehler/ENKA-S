# --- Project description ---
cmake_minimum_required(VERSION 3.24)
project(ENKA-S VERSION 2.0 LANGUAGES CXX)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# --- C++ Standard ---
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- Qt setup ---
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 6.2 REQUIRED COMPONENTS
    Widgets
    OpenGLWidgets
    Gui
    Charts
    Test
)

# --- Dependencies ---
include(FetchContent)

FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.17.0.zip
  DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)

set(gtest_force_shared_crt ON CACHE BOOL "Use shared CRT on Windows" FORCE)
FetchContent_MakeAvailable(googletest)

# --- Subdirectories ---
add_subdirectory(core)
add_subdirectory(app)

# --- Testing ---
option(ENKAS_BUILD_TESTS "Build the unit tests" ON)
if(ENKAS_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
