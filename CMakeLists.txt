cmake_minimum_required(VERSION 3.20)
project(CAVE)

set(CMAKE_C_STANDARD 99)

include_directories(PRIVATE include)
add_subdirectory(src)
add_subdirectory(tests)
