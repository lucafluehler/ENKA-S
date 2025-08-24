# --- Project description ---
cmake_minimum_required(VERSION 3.24)
project(ENKA-S VERSION 2.0.0 LANGUAGES CXX)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# --- C++ Standard ---
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- Qt setup ---
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 6.7 REQUIRED COMPONENTS
    Widgets
    OpenGLWidgets
    Gui
    Charts
    Test
)

# --- CPack Configuration for Packaging ---
include(CPack)

set(CPACK_PACKAGE_NAME "${PROJECT_NAME}")
set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "A simple desktop application for running and analyzing N-body particle simulations.")
set(CPACK_PACKAGE_VENDOR "ENKA-S Project")
set(CPACK_PACKAGE_CONTACT "https://github.com/lucafluehler/ENKA-S/issues")

set(CPACK_NSIS_MUI_ICON "${CMAKE_SOURCE_DIR}/app/resources/icons/icon.ico")
set(CPACK_NSIS_MUI_UNIICON "${CMAKE_SOURCE_DIR}/app/resources/icons/icon.ico")

set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/LICENSE")
set(CPACK_RESOURCE_FILE_README "${CMAKE_SOURCE_DIR}/README.md")

set(CPACK_PACKAGE_INSTALL_DIRECTORY "${CPACK_PACKAGE_NAME}")

if(WIN32)
    set(CPACK_GENERATOR "NSIS")
    set(CPACK_INSTALLED_DIRECTORIES "${CMAKE_BINARY_DIR}/AppDir;/")
    set(CPACK_NSIS_CREATE_ICONS_EXTRA "CreateShortCut '$DESKTOP\\${CPACK_PACKAGE_NAME}.lnk' '$INSTDIR\\bin\\enkas.exe'")
    file(READ "${CMAKE_SOURCE_DIR}/LICENSE" LICENSE_TEXT)
    set(CPACK_NSIS_LICENSE_TEXT "${LICENSE_TEXT}")
elseif(APPLE)
    set(CPACK_GENERATOR "DragNDrop")
else()
    # For Linux, our GitHub Action will create an AppImage externally.
    set(CPACK_GENERATOR "DEB")
endif()

# --- Dependencies ---
include(FetchContent)

FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.17.0.zip
  DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)

set(gtest_force_shared_crt ON CACHE BOOL "Use shared CRT on Windows" FORCE)
set(INSTALL_GTEST OFF CACHE BOOL "Disable installation of gtest" FORCE)
FetchContent_MakeAvailable(googletest)

# --- Subdirectories ---
add_subdirectory(core)
add_subdirectory(app)

# --- Generate deployment script ---
qt_generate_deploy_app_script(
    TARGET enkas
    OUTPUT_SCRIPT "${CMAKE_BINARY_DIR}/deploy_script.cmake"
    NO_UNSUPPORTED_PLATFORM_ERROR
)

install(SCRIPT "${CMAKE_BINARY_DIR}/deploy_script.cmake")

# --- CPack Installation Rules ---
install(TARGETS enkas
    RUNTIME DESTINATION bin 
    BUNDLE DESTINATION .  
)

# --- Linux specific installation rules ---
if(UNIX AND NOT APPLE)
    install(
        FILES app/resources/icons/icon.png
        DESTINATION share/icons/hicolor/256x256/apps
        RENAME ${PROJECT_NAME}.png
    )
    install(
        FILES app/resources/shortcuts/ENKA-S.desktop
        DESTINATION share/applications
        RENAME ${PROJECT_NAME}.desktop
    )
endif()

# --- Testing ---
option(ENKAS_BUILD_TESTS "Build the unit tests" ON)
if(ENKAS_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
