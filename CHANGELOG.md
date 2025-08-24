# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]


---

## [v2.0.0] - 2025-09-01

### Added
- **Simulation Replay Mode:** Allows users to load and review completed simulations.
    - Full playback controls including play/pause, variable speed, frame-by-frame stepping (forward and backward) aswell as timeline seeking.
    - Complete diagnostic charts are populated with data from the entire simulation run for detailed analysis.
- **Automated CI/CD pipeline:** Integrated GitHub Actions for full Continuous Integration and Delivery.
    - Automated builds on Windows, macOS, and Linux for every push and pull request.
    - Automated execution of the entire test suite on all three platforms.
    - Automated code formatting checks using clang-format.
    - Automated release packaging that creates installers for Windows (.exe), macOS (.dmg), and Linux (.AppImage).
- **Testing Suite:** Introduced unit testing using Google Test and Google Mock for various parts of the app and core.
- **Documentation:** Added README, CHANGELOG and better in-code documentation with Doxygen.
- **Logging:** Added logging system and dedicated logging tab in the GUI.

### Changed
- **Project architecture:** 
    - Complete overhaul and splitting of the core library and the app part.
    - Introduced a Model-View-Presenter architecture for the GUI code.
    - Introduced a composition root for dependency injection for proper decoupling and better unit testing.
- **Rendering Engine:**
    - Major performance improvements due to the proper use of GLSL and more modern OpenGL features.
    - Particle rendering is now performed on the GPU using instanced drawing.
- **Build System:** Created a more modular structure and better seperation.
- **Core:**
    - Migrated all core logic into a dedicated namespace.
    - Refactored multiple files to better reflect their purpose and split them into dedicated files.
    - Use a Structure-of-Arrays (SoA) layout for the system data structure.
    - Take away the memory management responsibility of the simulation engine to allow for more efficient memory pre-allocation managed by the user-side.
- **Configuration Management:** Switched from a CSV settings file to a more robust JSON format.
- **Language:** Switch from German to English for the default interface language.

---

## [v1.0.0] - 2025-07-04

### Added
- **Initial Project Release:** This version represents the state of the project as submitted for a high school graduation project.
- **Simulation Core:** Implementation of five N-body simulation algorithms.
- **Initial Conditions:** Procedural generation of initial system conditions with six models or importing a file.
- **Graphical User Interface:**
    - Desktop application built with C++20 and Qt 6.
    - A "New Simulation" tab for configuring all generation and simulation parameters.
    - A "Load Simulation" tab for re-loading data from a previous run (unfinished).
    - A dedicated "Simulation Window" for live visualization.
- **Rendering:**
    - Real-time 2D projection of the 3D particle system using `QOpenGLWidget`.
    - Interactive camera controls (pan, zoom, orbit) and basic rendering settings.
- **Data Handling:**
    - Functionality to save simulation results (system state, diagnostics) to CSV files for analysis and persistence.