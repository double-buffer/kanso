add_executable(Kernel UnityBuild.c Boot.s)

target_link_options(Kernel PRIVATE
    --target=riscv64-unknown-elf   # if your toolchain doesn’t add this automatically
    -nostdlib
    -Wl,-T${CMAKE_CURRENT_SOURCE_DIR}/Kernel.ld
)

set_target_properties(Kernel PROPERTIES
    OUTPUT_NAME "kernel"     # base name
    SUFFIX      ".elf"       # add the .elf extension
)

