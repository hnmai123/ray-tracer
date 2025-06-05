cmake_minimum_required(VERSION 3.23)
project(raytracer)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add common warnings
add_compile_options(-Wall -Wextra -Wpedantic)

# Automatically collect all headers and source files
file(GLOB HEADERS
    "*.h"
)

file(GLOB SOURCES
    "*.cpp"
    "main.cpp" # ensure main is included explicitly if needed
)

# Create the executable
add_executable(raytracer ${SOURCES} ${HEADERS})
