cmake_minimum_required(VERSION 3.12)
project(CrazyCurves)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Define source files
set(SOURCES
    src/CrazyCurves.cpp
    src/CurveInterpolation.cpp
    src/CurvesData.cpp
    src/CurvePresets.cpp
    src/PresetManagerUI.cpp
)

# Define header files
set(HEADERS
    src/CrazyCurves.hpp
    src/CurveInterpolation.hpp
    src/CurvesData.hpp
    src/CurvePresets.hpp
    src/PresetManagerUI.hpp
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
else()
    set(PLATFORM_LIBS "")
endif()

# Create shared library
add_library(CrazyCurves SHARED ${SOURCES} ${HEADERS})

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