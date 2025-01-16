
cmake_minimum_required(VERSION 3.10)
project(window_sieve)

set(CMAKE_CXX_FLAGS_RELEASE "${RELEASE}")
set(CMAKE_CXX_FLAGS_DEBUG "${DEBUG}")

# Specify header files located in the include directory
include_directories(include)

# Add executable for window_sieve
add_executable(window_sieve src/window_sieve.c 
src/hardware_info.c 
src/trial_division.c
src/prime_convert.c
src/prime_file.c
src/prime_formatting.c
)

# Add executable for prime_gap_analyzer
add_executable(prime_gap_analyzer src/prime_gap_analyzer.c)

# Add executable for prime_gap_analyzer
add_executable(prime_convert src/prime_convert.c
                             src/prime_convert.c 
                             src/prime_formatting.c 
                             src/prime_file.c 
                             src/trial_division.c 

)
target_compile_definitions(prime_convert PRIVATE PRIME_CONVERT_MAIN)  

# Set the output directory for executables
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)

# Ensure the output directory exists
file(MAKE_DIRECTORY ${EXECUTABLE_OUTPUT_PATH})

# Set the runtime output directory for each target
set_target_properties(window_sieve prime_gap_analyzer PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${EXECUTABLE_OUTPUT_PATH}
)

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
target_include_directories(prime_convert PRIVATE include)

# Custom clean target
set(CLEAN_FILES
    ${CMAKE_BINARY_DIR}/bin/window_sieve
    ${CMAKE_BINARY_DIR}/bin/prime_gap_analyzer
    ${CMAKE_BINARY_DIR}/bin/prime_convert
    ${CMAKE_BINARY_DIR}/CMakeCache.txt
    ${CMAKE_BINARY_DIR}/cmake_install.cmake
    ${CMAKE_BINARY_DIR}/Makefile
)

# Test targets
add_custom_target(test_window_sieve
    COMMAND ${EXECUTABLE_OUTPUT_PATH}/window_sieve -v -w 100000 -u 1000000 -p
    DEPENDS window_sieve
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMENT "Running window_sieve test"
)

add_custom_target(test_prime_gap_analyzer
    COMMAND ${EXECUTABLE_OUTPUT_PATH}/prime_gap_analyzer -n 492113 -c 114
    DEPENDS prime_gap_analyzer
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMENT "Running prime_gap_analyzer test"
)

add_custom_target(test_prime_convert
    COMMAND ${EXECUTABLE_OUTPUT_PATH}/prime_convert -i primes.bin -o primes.csv -fpn
    DEPENDS prime_convert
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMENT "Running prime_convert test"
)

# Add a custom target to run all tests
add_custom_target(test_all
    DEPENDS test_window_sieve test_prime_gap_analyzer test_prime_convert
)
