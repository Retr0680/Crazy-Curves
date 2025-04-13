cmake_minimum_required(VERSION 3.12)
project(CrazyCurves)

# Set C++ standard to C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Define core source files that don't depend on AE SDK
set(CORE_SOURCES
    src/CurveData.cpp
    src/CurveInterpolation.cpp
)

# Create shared library
add_library(CrazyCurves SHARED ${CORE_SOURCES})

# Include directories
target_include_directories(CrazyCurves PRIVATE
    "${CMAKE_SOURCE_DIR}/src"
)