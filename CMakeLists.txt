cmake_minimum_required(VERSION 3.5)

project(enkas VERSION 1.0 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)
#set(CMAKE_CXX_STANDARD_REQUIRED ON)

#find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt6 REQUIRED COMPONENTS Widgets)
find_package(Qt6 REQUIRED COMPONENTS OpenGLWidgets)
find_package(Qt6 REQUIRED COMPONENTS Gui)
find_package(Qt6 REQUIRED COMPONENTS Charts)
find_package(OpenGL REQUIRED)

#qt_standard_project_setup()

set(CMAKE_AUTOUIC_SEARCH_PATHS ui/)

set( HEADERS
    include/GUI/main_window.h
    include/GUI/render_settings_widget.h
    include/GUI/simulation_window.h
    include/GUI/line_chart_widget.h
    include/GUI/diagnostics_tab.h
    include/GUI/particle_renderer.h
    include/GUI/load_simulation_tab.h
    include/GUI/new_simulation_tab.h
    include/GUI/fancy_double_spin_box.h
    include/GUI/file_check_icon.h
    include/GUI/system_preview.h

    include/Logic/simulation_worker.h
    include/Logic/data_saver.h
    include/Logic/file_checker.h
    include/Logic/simulation_manager.h

    include/Rendering/camera.h

    include/Settings/settings.h
    include/Settings/data_settings.h
    include/Settings/render_settings.h
    include/Settings/generation_settings.h
    include/Settings/simulation_settings.h

    include/Simulation/simulation_factory.h
    include/Simulation/simulator.h
    include/Simulation/sm_euler.h
    include/Simulation/sm_leapfrog.h
    include/Simulation/sm_hermite.h
    include/Simulation/sm_hits.h
    include/Simulation/sm_hacs.h
    include/Simulation/sm_bhleapfrog.h
    include/Simulation/Utils/bh_tree.h

    include/SystemGeneration/generation_factory.h
    include/SystemGeneration/generator.h
    include/SystemGeneration/gm_file.h
    include/SystemGeneration/gm_normal_sphere.h
    include/SystemGeneration/gm_uniform_cube.h
    include/SystemGeneration/gm_uniform_sphere.h
    include/SystemGeneration/gm_plummer_sphere.h
    include/SystemGeneration/gm_collision_model.h
    include/SystemGeneration/gm_flyby_model.h
    include/SystemGeneration/gm_spiral_galaxy.h

    include/Utils/utils.h


    include/Utils/Data/analytics_data.h
    include/Utils/Data/diagnostics_data.h
    include/Utils/Data/render_data.h
    include/Utils/Data/data_ptr.h

    include/Utils/GeometricAlgebra/geometric_algebra.h
    include/Utils/GeometricAlgebra/bivector3d.h
    include/Utils/GeometricAlgebra/rotor3d.h
    include/Utils/GeometricAlgebra/vector3d.h


)

set( SOURCES
    src/main.cpp

    src/Utils/GeometricAlgebra/bivector3d.cpp
    src/Utils/GeometricAlgebra/rotor3d.cpp
    src/Utils/GeometricAlgebra/vector3d.cpp

    src/GUI/main_window.cpp
    src/GUI/render_settings_widget.cpp
    src/GUI/line_chart_widget.cpp
    src/GUI/simulation_window.cpp
    src/GUI/diagnostics_tab.cpp
    src/GUI/particle_renderer.cpp
    src/GUI/load_simulation_tab.cpp
    src/GUI/new_simulation_tab.cpp
    src/GUI/fancy_double_spin_box.cpp
    src/GUI/file_check_icon.cpp
    src/GUI/system_preview.cpp

    src/Logic/simulation_worker.cpp
    src/Logic/data_saver.cpp
    src/Logic/file_checker.cpp
    src/Logic/simulation_manager.cpp

    src/Settings/settings.cpp

    src/Simulation/simulation_factory.cpp
    src/Simulation/sm_euler.cpp
    src/Simulation/sm_leapfrog.cpp
    src/Simulation/sm_hermite.cpp
    src/Simulation/sm_hits.cpp
    src/Simulation/sm_hacs.cpp
    src/Simulation/sm_bhleapfrog.cpp
    src/Simulation/Utils/bh_tree.cpp

    src/SystemGeneration/generation_factory.cpp
    src/SystemGeneration/gm_file.cpp
    src/SystemGeneration/gm_normal_sphere.cpp
    src/SystemGeneration/gm_uniform_cube.cpp
    src/SystemGeneration/gm_plummer_sphere.cpp
    src/SystemGeneration/gm_spiral_galaxy.cpp
    src/SystemGeneration/gm_collision_model.cpp
    src/SystemGeneration/gm_flyby_model.cpp
    src/SystemGeneration/gm_uniform_sphere.cpp
)

set( FORMS
    ui/main_window.ui
    ui/render_settings_widget.ui
    ui/simulation_window.ui
    ui/diagnostics_tab.ui
    ui/load_simulation_tab.ui
    ui/new_simulation_tab.ui
)

set( RESOURCES
    ui/resources.qrc
)

qt_add_executable(enkas
    ${SOURCES}
    ${HEADERS}
    ${FORMS}
    ${RESOURCES}
)

# Needed for #include "header.h" instead of #include "test/header.h"
target_include_directories(enkas PRIVATE
    "${PROJECT_SOURCE_DIR}/include"
    "${PROJECT_SOURCE_DIR}/include/Utils"
    "${PROJECT_SOURCE_DIR}/include/Utils/Data"
    "${PROJECT_SOURCE_DIR}/include/Utils/GeometricAlgebra"
    "${PROJECT_SOURCE_DIR}/include/GUI"
    "${PROJECT_SOURCE_DIR}/include/Logic"
    "${PROJECT_SOURCE_DIR}/include/Rendering"
    "${PROJECT_SOURCE_DIR}/include/Settings"
    "${PROJECT_SOURCE_DIR}/include/Simulation"
    "${PROJECT_SOURCE_DIR}/include/Simulation/Utils"
    "${PROJECT_SOURCE_DIR}/include/SystemGeneration"
)

target_link_libraries(enkas PRIVATE Qt6::OpenGLWidgets)
target_link_libraries(enkas PRIVATE Qt6::Widgets)
target_link_libraries(enkas PRIVATE Qt6::Gui)
target_link_libraries(enkas PRIVATE Qt6::Charts)
target_link_libraries(enkas PRIVATE OpenGL::GL)

set_target_properties(enkas PROPERTIES
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
#if(${QT_VERSION} VERSION_LESS 6.1.0)
#  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.enkas)
#endif()
#set_target_properties(enkas PROPERTIES
#    ${BUNDLE_ID_OPTION}
#    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
#    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
#    MACOSX_BUNDLE TRUE
#    WIN32_EXECUTABLE TRUE
#)

#include(GNUInstallDirs)
#install(TARGETS enkas
#    BUNDLE DESTINATION .
#    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
#    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
#)

#qt_finalize_executable(enkas)
