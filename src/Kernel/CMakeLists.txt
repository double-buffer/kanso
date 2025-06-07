# TODO: Add the correct platform
add_subdirectory(Platforms/RiscV)
add_dependencies(Kernel generate_version_header)
add_dependencies(KernelTest generate_version_header)
