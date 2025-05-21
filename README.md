# Rush Hour Solver

A visual solver for the **Rush Hour** puzzle game, implemented in C++ using **raylib** and **raygui** for 3D rendering and GUI. This project simulates and solves puzzle configurations while visualizing car movements in a smooth, interactive 3D interface.

## Build Instructions

### 🛠️ Prerequisites

- CMake ≥ 3.21
- A C++17 compatible compiler
- [vcpkg](https://github.com/microsoft/vcpkg)
- Ninja (recommended)
- Raylib + Raygui (installed via vcpkg)

### 📦 Build

On Windows:
```
cmake --preset windows-debug
cmake --build --preset windows-debug
```

On Linux:
```
cmake --preset linux-debug
cmake --build --preset linux-debug
```

Built binaries will be placed in bin/\<platform\>-\<configuration\>/

## How to Use

### 🏃 Running

1. If the desired platform and configuration is present, build it using the build instructions
2. Open the directory bin/\<platform\>-\<configuration\>/
3. Run the rush-hour-solver executable located in the directory

### 🕹️ Using
1. Choose a configuration file and algorithm with the sidebar on the left
2. Press solve and wait while the program tries to solve the configuration
3. Press next to manually view the next step or press auto to autoplay the steps
4. Click save to save the output in a txt file and return to go back to the configuration menu

# Contributors
- Arlow Emmanuel Hergara (13523161)
