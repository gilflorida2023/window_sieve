cmake_minimum_required(VERSION 3.10)
project(window_sieve)

set(CMAKE_CXX_FLAGS_RELEASE "${RELEASE}")
set(CMAKE_CXX_FLAGS_DEBUG "${DEBUG}")

add_executable(window_sieve window_sieve.c)

if(CMAKE_BUILD_TYPE STREQUAL "Release")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS_RELEASE}")
elseif(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS_DEBUG}")
else()
    message(FATAL_ERROR "Unsupported build type")
endif()
