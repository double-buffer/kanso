add_executable(Kernel UnityBuild.c Boot.S SupervisorTrapEntry.S)

target_link_libraries(Kernel PRIVATE Common)

target_link_options(Kernel PRIVATE
    --target=riscv64-unknown-elf   # if your toolchain doesn’t add this automatically
    -nostdlib
    -Wl,-T${CMAKE_CURRENT_SOURCE_DIR}/Kernel.ld
)

set_target_properties(Kernel PROPERTIES
    OUTPUT_NAME "kernel"     # base name
    SUFFIX      ".elf"       # add the .elf extension
)


add_executable(KernelTest UnityBuild.c Boot.S SupervisorTrapEntry.S)

target_link_libraries(KernelTest PRIVATE Common)

target_link_options(KernelTest PRIVATE
    --target=riscv64-unknown-elf   # if your toolchain doesn’t add this automatically
    -nostdlib
    -Wl,-T${CMAKE_CURRENT_SOURCE_DIR}/Kernel.ld
)

target_compile_definitions(KernelTest PRIVATE BUILD_TESTS)

set_target_properties(KernelTest PROPERTIES
    OUTPUT_NAME "kernel-test"     # base name
    SUFFIX      ".elf"       # add the .elf extension
)

