add_library(Common INTERFACE)
add_dependencies(Common generate_version_header)

target_include_directories(Common INTERFACE .)
target_include_directories(Common INTERFACE ${CMAKE_BINARY_DIR}/src/Common)

