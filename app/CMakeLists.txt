file(GLOB_RECURSE APP_SOURCES src/*.cpp src/*.h)
file(GLOB_RECURSE APP_FORMS resources/*.ui)
file(GLOB_RECURSE APP_RESOURCES resources/*.qrc)

add_executable(enkas
    ${APP_SOURCES}
    ${APP_FORMS}
    ${APP_RESOURCES}
)

set_target_properties(enkas PROPERTIES AUTOUIC_SEARCH_PATHS "${CMAKE_CURRENT_SOURCE_DIR}/resources")

target_link_libraries(enkas PRIVATE
    enkas-core

    Qt6::Widgets
    Qt6::OpenGLWidgets
    Qt6::Gui
    Qt6::Charts
)

set_target_properties(enkas PROPERTIES
    WIN32_EXECUTABLE TRUE
    MACOSX_BUNDLE TRUE
)
