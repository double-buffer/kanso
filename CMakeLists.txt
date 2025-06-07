cmake_minimum_required(VERSION 4.0)

set(CMAKE_C_STANDARD "23")

project(Kanso LANGUAGES C ASM)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include(cmake/utils.cmake)

set(VERSION_HEADER_PATH "${CMAKE_BINARY_DIR}/src/Common/Version.h")
file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/src/Common")

add_custom_target(generate_version_header ALL
  COMMAND ${CMAKE_COMMAND} -DOUT_FILE=${VERSION_HEADER_PATH}
                           -P "${CMAKE_SOURCE_DIR}/cmake/generate_version_header.cmake"
  COMMENT "Regenerating version.h from Git tag"
)

add_subdirectory(src/Common)
add_subdirectory(src/Kernel)

