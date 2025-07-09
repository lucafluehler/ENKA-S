# --- Source files ---
file(GLOB_RECURSE CORE_SOURCES src/*.cpp)
file(GLOB_RECURSE CORE_HEADERS include/*.h)

add_library(enkas-core STATIC
    ${CORE_SOURCES}
    ${CORE_HEADERS}
)

# --- Include directories ---
target_include_directories(enkas-core PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

# --- C++ Standard ---
target_compile_features(enkas-core PUBLIC cxx_std_23)
