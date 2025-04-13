cmake_minimum_required(VERSION 3.12)
project(CrazyCurves)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Check if AE SDK exists
if(EXISTS "${CMAKE_SOURCE_DIR}/extern/AfterEffectsSDK")
    set(AE_SDK_PATH "${CMAKE_SOURCE_DIR}/extern/AfterEffectsSDK")
    add_definitions(-DWITH_AE_SDK)
else()
    message(WARNING "Adobe After Effects SDK not found - building core components only")
endif()

# Include directories
include_directories(
    "${CMAKE_SOURCE_DIR}/src"
)

if(DEFINED AE_SDK_PATH)
    include_directories(
        "${AE_SDK_PATH}/Examples/Headers"
        "${AE_SDK_PATH}/Examples/Util"
        "${AE_SDK_PATH}/Examples/Resources"
    )
endif()

# Define source files
set(SOURCES
    src/CurveData.cpp
    src/CurveInterpolation.cpp
)

if(DEFINED AE_SDK_PATH)
    list(APPEND SOURCES
        src/CrazyCurves.cpp
        src/PreviewWindow.cpp
    )
endif()

# Create shared library
add_library(CrazyCurves SHARED ${SOURCES})

# Set output extension for AE plugin
if(WIN32 AND DEFINED AE_SDK_PATH)
    set_target_properties(CrazyCurves PROPERTIES SUFFIX ".aex")
endif()