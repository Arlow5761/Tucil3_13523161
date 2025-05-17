cmake_minimum_required(VERSION 3.21)

# Enable vcpkg toolchain
if(DEFINED ENV{VCPKG_ROOT})
    set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake"
        CACHE STRING "Vcpkg toolchain file")
endif()

project(rush-hour-solver)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Detect platform and build configuration for folder naming
if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    set(PLATFORM_NAME "windows")
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    set(PLATFORM_NAME "linux")
else()
    set(PLATFORM_NAME "unknown")
endif()

# Set output directories for executables and libraries
string(TOLOWER "${CMAKE_BUILD_TYPE}" CONFIG)
set(OUTPUT_PATH "${CMAKE_SOURCE_DIR}/bin/${PLATFORM_NAME}-${CONFIG}")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${OUTPUT_PATH})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${OUTPUT_PATH})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${OUTPUT_PATH})

# Add executable
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS src/*.cpp)
add_executable(rush-hour-solver ${SOURCES})

# Link libraries (automatically handled by vcpkg + find_package)
find_package(raylib CONFIG REQUIRED)
target_link_libraries(rush-hour-solver PRIVATE raylib)
