crazy-curves
├── src
│   ├── main.cpp
│   ├── plugin.h
│   ├── plugin.cpp
│   ├── effects
│   │   ├── curves.h
│   │   └── curves.cpp
│   └── utils
│       ├── math.h
│       └── math.cpp
├── include
│   └── ae_sdk
├── CMakeLists.txt
│   cmake_minimum_required(VERSION 3.12)
│   project(CrazyCurves)
│   
│   # Set C++ standard
│   set(CMAKE_CXX_STANDARD 17)
│   set(CMAKE_CXX_STANDARD_REQUIRED ON)
│   
│   # After Effects SDK path - update this to your SDK location
│   set(AE_SDK_PATH "${CMAKE_SOURCE_DIR}/include/ae_sdk")
│   
│   # Include directories
│   include_directories(
│       ${CMAKE_SOURCE_DIR}/src
│       ${AE_SDK_PATH}/Headers
│       ${AE_SDK_PATH}/Util
│   )
│   
│   # Source files
│   set(SOURCES
│       src/main.cpp
│       src/plugin.cpp
│       src/effects/curves.cpp
│       src/utils/math.cpp
│   )
│   
│   # Header files
│   set(HEADERS
│       src/plugin.h
│       src/effects/curves.h
│       src/utils/math.h
│   )
│   
│   # Create shared library
│   add_library(CrazyCurves SHARED
│       ${SOURCES}
│       ${HEADERS}
│   )
│   
│   # Set output directory and naming conventions
│   set_target_properties(CrazyCurves PROPERTIES
│       PREFIX ""
│       SUFFIX ".aex"
│       RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
│       LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
│   )
│   
│   # Link After Effects SDK libraries
│   target_link_libraries(CrazyCurves
│       ${AE_SDK_PATH}/Lib/Win/AE_SDK_Libs.lib
│   )
│   
│   # Windows-specific settings
│   if(WIN32)
│       add_definitions(-DWIN32 -D_WINDOWS)
│       set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} /SUBSYSTEM:WINDOWS")
│   endif()
├── LICENSE
└── README.md