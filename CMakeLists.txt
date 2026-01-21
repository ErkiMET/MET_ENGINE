
cmake_minimum_required(VERSION 4.0.1)

# Project name
project(MET_ENGINE)

# C++ standard
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# executable
add_executable(MET_ENGINE WIN32 src/window.cpp)

# Link libraries
target_link_libraries(MET_ENGINE PRIVATE
    user32
    kernel32
    gdi32
)

# define UNICODE for preprocessor
target_compile_definitions(MET_ENGINE PRIVATE UNICODE)

