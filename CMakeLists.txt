cmake_minimum_required(VERSION 4.0)

set(CMAKE_C_STANDARD "23")

project(Kanso)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_subdirectory(src/Kernel)
