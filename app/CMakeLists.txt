# --- Source files ---
file(GLOB_RECURSE APP_LIB_SOURCES CONFIGURE_DEPENDS "src/*.cpp")
list(FILTER APP_LIB_SOURCES EXCLUDE REGEX ".*/main\\.cpp$")
file(GLOB_RECURSE APP_LIB_HEADERS CONFIGURE_DEPENDS "src/*.h")
file(GLOB_RECURSE APP_FORMS CONFIGURE_DEPENDS "resources/*.ui")
file(GLOB APP_RESOURCES CONFIGURE_DEPENDS "resources/resources.qrc")

add_library(enkas-app
    ${APP_LIB_SOURCES}
    ${APP_LIB_HEADERS}
    ${APP_FORMS}
    ${APP_RESOURCES}
)

# --- Target properties ---
set_target_properties(enkas-app PROPERTIES
    AUTOUIC_SEARCH_PATHS "${CMAKE_CURRENT_SOURCE_DIR}/resources"
)

# --- Link dependencies ---
target_link_libraries(enkas-app PUBLIC
    enkas-core
    Qt6::Widgets
    Qt6::OpenGLWidgets
    Qt6::Gui
    Qt6::Charts
    Qt6::Concurrent
)

# --- Include directories ---
target_include_directories(enkas-app PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/vendor
)

# --- C++ standard ---
target_compile_features(enkas-app PRIVATE cxx_std_23)

# --- Wrap app into enkas executable ---
add_executable(enkas
    src/main.cpp
)

set_target_properties(enkas PROPERTIES
    WIN32_EXECUTABLE TRUE
    MACOSX_BUNDLE TRUE
)

target_link_libraries(enkas PRIVATE
    enkas-app
)
