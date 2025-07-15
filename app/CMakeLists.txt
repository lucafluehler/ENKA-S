# --- Source files ---
file(GLOB_RECURSE APP_SOURCES CONFIGURE_DEPENDS src/*.cpp src/*.h)
file(GLOB_RECURSE APP_FORMS CONFIGURE_DEPENDS resources/*.ui)
file(GLOB_RECURSE APP_RESOURCES CONFIGURE_DEPENDS resources/*.qrc)

add_executable(enkas
    ${APP_SOURCES}
    ${APP_FORMS}
    ${APP_RESOURCES}
)

# --- Target properties ---
set_target_properties(enkas PROPERTIES 
    AUTOUIC_SEARCH_PATHS "${CMAKE_CURRENT_SOURCE_DIR}/resources"
    WIN32_EXECUTABLE TRUE
    MACOSX_BUNDLE TRUE
)

# --- Link dependencies ---
target_link_libraries(enkas PRIVATE
    enkas-core

    Qt6::Widgets
    Qt6::OpenGLWidgets
    Qt6::Gui
    Qt6::Charts
)

# --- Include directories ---
target_include_directories(enkas PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/vendor
)

# --- C++ standard ---
target_compile_features(enkas PRIVATE cxx_std_23)
