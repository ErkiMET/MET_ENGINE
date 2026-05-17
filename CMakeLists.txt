
cmake_minimum_required(VERSION 4.2.1)

# Project name
project(MET_ENGINE)

# C++ standard
set(CMAKE_CXX_STANDARD 23)
set(CXX_SCAN_FOR_MODULES)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# executable
add_executable(MET_ENGINE WIN32 src/window.cpp)

# Compile Features

target_sources(MET_ENGINE 
    PUBLIC
    FILE_SET CXX_MODULES
    BASE_DIRS src
    FILES src/Win32DSound.ixx
)

# Link libraries
target_link_libraries(MET_ENGINE PRIVATE user32 kernel32 gdi32)