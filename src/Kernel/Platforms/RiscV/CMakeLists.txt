add_executable(Kernel UnityBuild.c Boot.S KernelTrapEntry.S)

target_link_libraries(Kernel PRIVATE Common)

target_link_options(Kernel PRIVATE
    -Wl,-T${CMAKE_CURRENT_SOURCE_DIR}/Kernel.ld
)

set_target_properties(Kernel PROPERTIES
    OUTPUT_NAME "kernel"     # base name
    SUFFIX      ".elf"       # add the .elf extension
)

add_custom_command(TARGET Kernel POST_BUILD
    COMMAND "${CMAKE_OBJCOPY}"
            --output-target=binary               # -O binary
            --binary-architecture=riscv:${_MARCH}
            $<TARGET_FILE:Kernel>                # kernel.elf (source)
            $<TARGET_FILE_DIR:Kernel>/kernel.bin # kernel.bin (dest)
    COMMENT "Generating flat binary: kernel.bin"
)


add_executable(KernelTest UnityBuild.c Boot.S KernelTrapEntry.S)

target_link_libraries(KernelTest PRIVATE Common)

target_link_options(KernelTest PRIVATE
    -Wl,-T${CMAKE_CURRENT_SOURCE_DIR}/Kernel.ld
)

target_compile_definitions(KernelTest PRIVATE BUILD_TESTS)

set_target_properties(KernelTest PROPERTIES
    OUTPUT_NAME "kernel-test"     # base name
    SUFFIX      ".elf"       # add the .elf extension
)

