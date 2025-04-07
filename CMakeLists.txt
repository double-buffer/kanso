cmake_minimum_required(VERSION 4.0)

set(CMAKE_C_STANDARD "23")
Set(CLANG_COMPILE_OPTIONS -Wall -Wextra -Wpedantic)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

Project(Kanso LANGUAGES C)
add_executable(Kanso src/UnityBuild.c)
