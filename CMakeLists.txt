cmake_minimum_required(VERSION 3.12)
project(CrazyCurves)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Define source files
set(SOURCES
    src/CrazyCurves.cpp
    src/ChannelManager.cpp
    src/CurveCache.cpp
    src/CurveHistory.cpp
    src/CurveInterpolation.cpp
    src/CurvePresets.cpp
    src/CurvesData.cpp
    src/CurvesUI.cpp
    src/PresetManagerUI.cpp
    src/PreviewWindow.cpp
    src/DrawingUtils.cpp
)

# Define header files
set(HEADERS
    src/CrazyCurves.h
)

# Adobe After Effects SDK path - Update this to your SDK path
set(AE_SDK_PATH "${CMAKE_SOURCE_DIR}/extern/AfterEffectsSDK" CACHE PATH "Path to After Effects SDK")

# Include directories
include_directories(
    ${AE_SDK_PATH}/Headers
    ${AE_SDK_PATH}/Headers/SP
    ${AE_SDK_PATH}/Util
    ${CMAKE_SOURCE_DIR}/src
)

# Platform-specific settings
if(WIN32)
    add_definitions(-D_WINDOWS)
    set(CMAKE_SHARED_LIBRARY_PREFIX "")
    set(PLATFORM_LIBS
        kernel32
        user32
        gdi32
        shell32
    )
endif()

# Add AE SDK required definitions
add_definitions(
    -DWIN32
    -D_WINDOWS
    -D_USRDLL
    -D_CRT_SECURE_NO_WARNINGS
    -DISOLATION_AWARE_ENABLED
)

# Create shared library
add_library(CrazyCurves SHARED ${SOURCES} ${HEADERS})

# Add AE plugin extension
if(WIN32)
    set_target_properties(CrazyCurves PROPERTIES
        SUFFIX ".aex"
    )
endif()

# Link libraries
target_link_libraries(CrazyCurves PRIVATE ${PLATFORM_LIBS})

# Set output directory
set_target_properties(CrazyCurves PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

# Installation
install(TARGETS CrazyCurves
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
)