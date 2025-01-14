cmake_minimum_required(VERSION 3.10)
project(window_sieve)

set(CMAKE_CXX_FLAGS_RELEASE "${RELEASE}")
set(CMAKE_CXX_FLAGS_DEBUG "${DEBUG}")

# Explicitly list source files for window_sieve
set(WINDOW_SIEVE_SOURCES 
    src/window_sieve.c 
    src/hardware_info.c 
    src/trial_division.c
    src/prime_file.c
    src/prime_formatting.c
)

# Check if any sources were found for window_sieve
if(NOT WINDOW_SIEVE_SOURCES)
    message(FATAL_ERROR "No source files found for window_sieve.")
endif()

# Specify header files located in the include directory
include_directories(include)

# Add executable for window_sieve
add_executable(window_sieve ${WINDOW_SIEVE_SOURCES})

# Add executable for prime_gap_analyzer
add_executable(prime_gap_analyzer src/prime_gap_analyzer.c)

if(CMAKE_BUILD_TYPE STREQUAL "Release")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS_RELEASE}")
elseif(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS_DEBUG}")
else()
    message(FATAL_ERROR "Unsupported build type")
endif()

# Set include directories for the targets
target_include_directories(window_sieve PRIVATE include)
target_include_directories(prime_gap_analyzer PRIVATE include)

# Custom clean target
set(CLEAN_FILES
    ${CMAKE_BINARY_DIR}/window_sieve
    ${CMAKE_BINARY_DIR}/prime_gap_analyzer
    ${CMAKE_BINARY_DIR}/CMakeCache.txt
    ${CMAKE_BINARY_DIR}/cmake_install.cmake
    ${CMAKE_BINARY_DIR}/Makefile
)

add_custom_target(clean_custom
    COMMAND ${CMAKE_COMMAND} -E remove ${CLEAN_FILES}
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_BINARY_DIR}/CMakeFiles
    COMMENT "Cleaning custom files"
)

# Test targets
add_custom_target(test_window_sieve
    COMMAND time ./window_sieve -v -w 100000 -u 1000000 -p
    DEPENDS window_sieve
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMENT "Running window_sieve test"
)

add_custom_target(test_prime_gap_analyzer
    COMMAND time ./prime_gap_analyzer -n 492113 -c 114
    DEPENDS prime_gap_analyzer
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMENT "Running prime_gap_analyzer test"
)

# Add a custom target to run all tests
add_custom_target(test_all
    DEPENDS test_window_sieve test_prime_gap_analyzer
)

