cmake_minimum_required(VERSION 3.10)
project(window_sieve)

set(CMAKE_CXX_FLAGS_RELEASE "${RELEASE}")
set(CMAKE_CXX_FLAGS_DEBUG "${DEBUG}")

# Explicitly list source files
set(SOURCES 
    src/window_sieve.c 
    src/hardware_info.c 
    src/trial_division.c
)

# Check if any sources were found
if(NOT SOURCES)
    message(FATAL_ERROR "No source files found.")
endif()

# Specify header files located in the include directory
include_directories(include)

add_executable(window_sieve ${SOURCES})

if(CMAKE_BUILD_TYPE STREQUAL "Release")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS_RELEASE}")
elseif(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS_DEBUG}")
else()
    message(FATAL_ERROR "Unsupported build type")
endif()

# Set include directories for the target
target_include_directories(window_sieve PRIVATE include)

