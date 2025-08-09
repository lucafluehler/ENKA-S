# ENKA-S
<p align="center">
  <img src="https://img.shields.io/badge/C%2B%2B-23-blue.svg" alt="C++23">
  <img src="https://img.shields.io/badge/Qt-6-green.svg" alt="Qt 6">
  <img src="https://img.shields.io/badge/OpenGL-4.1-blueviolet.svg" alt="OpenGL">
  <img src="https://img.shields.io/badge/Build-CMake-orange.svg" alt="CMake">
  <img src="https://img.shields.io/badge/Tests-GoogleTest-brightgreen.svg" alt="GoogleTest">
  <img src="https://img.shields.io/badge/License-GPLv3-blue.svg" alt="License: GPLv3">
</p>

A simple desktop application for running and analyzing N-body particle simulations, built with C++23 and Qt 6.

## Gallery
    
<p align="center">
  <em>Home Screen</em><br/>
  <img src=".github/assets/enkas_home_screen.gif" alt="ENKA-S Home Screen" width="600">
</p>

<p align="center">
  <em>New Simulation Configuration</em><br/>
  <img src=".github/assets/enkas_new_simulation.gif" alt="ENKA-S New Simulation Tab" width="600">
</p>

<p align="center">
  <em>Loading a Previous Simulation</em><br/>
  <img src=".github/assets/enkas_load_simulation.gif" alt="ENKA-S Load Simulation Tab" width="600">
</p>

<p align="center">
  <em>Live Simulation &amp; Data Visualization</em><br/>
  <img src=".github/assets/enkas_simulation.gif" alt="ENKA-S Simulation Window" width="600">
</p>

## Key Features

-   **Comprehensive Simulation Setup:**
    -   **Initial Conditions:** Choose from 6 procedural generation models (e.g., *Plummer Sphere*, *Spiral Galaxy*) or import a system from a CSV file.
    -   **Simulation Algorithms:** Select from 5 different N-body integration methods, including classic direct-summation (*Euler*, *Leapfrog*, *Hermite*) and more advanced algorithms like *Hermite with Individual Time Steps (HITS)* and the *Barnes-Hut* tree-code.
    -   **Data Management:** Fine-tune data output, choosing whether to save system state, diagnostics, and settings to disk for later analysis.

-   **Live Simulation Monitoring:**
    -   Visualize the system in real-time with a hardware-accelerated **OpenGL** particle renderer.
    -   Analyze system stability with live-updating diagnostic charts for energy, momentum, and other physical properties.

-   **Interactive Simulation Replay:**
    -   Load and review completed simulations from saved data.
    -   Full playback controls including play/pause, variable speed, frame-by-frame stepping, and timeline seeking.
    -   View complete diagnostic charts for the entire simulation run.

## Getting Started

This guide will walk you through setting up your environment, building, and running the application. The building process was tested on Windows 11 with MSVC and on Arch Linux with GCC.

### Prerequisites

Before you begin, ensure you have the following software installed on your system:

-   **A C++23 Compliant Compiler:**
    -   GCC 13 or newer
    -   Clang 16 or newer
    -   MSVC v19.34 (Visual Studio 2022 17.4) or newer

-   **CMake:** Version 3.16 or higher.

-   **Qt 6:** Version 6.2 or higher. You can download the open-source version from the [official Qt website](https://www.qt.io/download-open-source). When installing, you must select the following modules:
    -   Qt Core
    -   Qt GUI
    -   Qt Widgets
    -   Qt OpenGL Widgets
    -   Qt Charts
    -   Qt Test (for building the tests)

    > **Note:**
    > -   If using the online installer on Windows, only `Qt Charts` needs to be selected from the additional libraries.
    > -   On Linux the required packages might be called `qt6-base qt6-declarative qt6-charts`.

### Building and Running

The project uses CMake for a straightforward, cross-platform build process.

1.  **Clone the repository:**
    ```bash
    git clone https://github.com/lucafluehler/ENKA-S.git
    cd ENKA-S
    ```

2.  **Configure the project with CMake:**
    This step generates the build files for your system (e.g., Makefiles, Visual Studio project). You must tell CMake where to find your Qt installation.

    ```bash
    cmake -B build -S . -DCMAKE_PREFIX_PATH=/path/to/your/Qt6/lib/cmake
    ```

    > **Important:** Replace `/path/to/your/Qt6/lib/cmake` with the actual path on your system.
    > -   On **Windows**, it might look like `C:/Qt/6.5.0/msvc2019_64/lib/cmake`.
    > -   On **macOS**, it might be `~/Qt/6.5.0/clang_64/lib/cmake`.
    > -   On **Linux**, it could be `/usr/lib/cmake/Qt6`.

3.  **Build the project:**
    This command compiles the source code and links the executables.

    ```bash
    cmake --build build --config Release
    ```

4.  **Run the application:**
    The final executable will be located inside the `build/app` directory.

    -   **On Linux/macOS:**
        ```bash
        ./build/app/enkas
        ```
    -   **On Windows:**
        ```bash
        ./build/app/Release/enkas.exe
        ```

    > **Important**
    > On Windows you might get errors that certain Qt6 related DLL files could not be found. In that case add the `bin` folder (eg. `C:\Qt\6.9.1\mingw_64\bin`) of your Qt installation to your PATH. Alternatively, install and use the tool `windeployqt`.

## Background
The first version of this project ([v1.0.0](https://github.com/lucafluehler/ENKA-S/releases/tag/v1.0.0)) was the original codebase developed for my high school graduation project. It was used to generate the data and graphics presented in the final paper. Since then, the project has undergone a major refactoring, improving its overall structure and adding previously missing features. With the release of [v2.0.0](https://github.com/lucafluehler/ENKA-S/releases/tag/v2.0.0), I decided to make the project public.

## License

This project is licensed under the GNU General Public License v3.0 (GPLv3). See the [LICENSE](LICENSE) file for details.

This application uses the [Qt framework](https://www.qt.io) under the terms of the GPLv3 license.

As required by the GPL:

- You are free to **use**, **modify**, and **distribute** this code under the same license.
- You must include the **full source code** of this application, including any modifications you make.
- You must provide users with a copy of the **GPLv3 license**.
- You must clearly state that this application uses the Qt framework.
- If applicable, you must provide a mechanism to **relink Qt libraries** if you dynamically link them.

### Qt Source Code

Qt is available under GPLv3. You can obtain a copy of the Qt source code here:
https://code.qt.io/cgit/qt/qt5.git/

### Acknowledgments

This application uses the Qt toolkit © The Qt Company Ltd and other contributors, licensed under the terms of the GNU General Public License v3.
