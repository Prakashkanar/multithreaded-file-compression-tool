cmake_minimum_required(VERSION 3.10)
project(FileCompressor)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -march=native")
set(CMAKE_CXX_FLAGS_DEBUG "-g -Wall -Wextra")

# Add the executable
add_executable(compressor
    main.cpp
    compressor.cpp
    compressor.h
)

# Link threading library
find_package(Threads REQUIRED)
target_link_libraries(compressor PRIVATE Threads::Threads)

# Optional: Enable optimizations for Release build
if(MSVC)
    target_compile_options(compressor PRIVATE /W4 /O2)
else()
    target_compile_options(compressor PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Set output directory
set_target_properties(compressor PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

# Print build info
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "Compiler: ${CMAKE_CXX_COMPILER}")
