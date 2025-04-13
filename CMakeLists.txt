cmake_minimum_required(VERSION 3.12)
project(CrazyCurves)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Adobe After Effects SDK path
set(AE_SDK_PATH "${CMAKE_SOURCE_DIR}/extern/AfterEffectsSDK")

# Include directories
include_directories(
    "${AE_SDK_PATH}/Examples/Headers"
    "${AE_SDK_PATH}/Examples/Util"
    "${AE_SDK_PATH}/Examples/Resources"
    "${CMAKE_SOURCE_DIR}/src"
)

# Define source files
set(SOURCES
    src/CrazyCurves.cpp
    src/CurveData.cpp
    src/CurveInterpolation.cpp
    src/PreviewWindow.cpp
)

# Platform-specific settings
if(WIN32)
    add_definitions(
        -DWIN32
        -D_WINDOWS
        -D_USRDLL
        -DISOLATION_AWARE_ENABLED
    )
endif()

# Create shared library
add_library(CrazyCurves SHARED ${SOURCES})

# Set output extension
if(WIN32)
    set_target_properties(CrazyCurves PROPERTIES SUFFIX ".aex")
endif()