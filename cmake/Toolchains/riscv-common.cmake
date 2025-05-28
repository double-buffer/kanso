set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR "riscv${RISCV_BITS}")
set(COMPILE_TARGET "riscv${RISCV_BITS}-unknown-elf")

include("${CMAKE_CURRENT_LIST_DIR}/clang.cmake")

set(_COMMON "-Wall -Wextra -Wpedantic -Wno-language-extension-token -fansi-escape-codes -fcolor-diagnostics --target=${COMPILE_TARGET} -fno-stack-protector -nostdinc -nostdlib -ffreestanding")

if(RISCV_BITS EQUAL 64)
    set(_MARCH "rv64gc")
    set(_MABI "lp64")
    set(_MODEL "-mcmodel=medany")
else()
    set(_MARCH "rv32gc")
    set(_MABI "ilp32")
    set(_MODEL "-mcmodel=medlow")
endif()

set(CMAKE_C_FLAGS "${_COMMON} -march=${_MARCH} -mabi=${_MABI} ${_MODEL}")
set(CMAKE_ASM_FLAGS "--target=${COMPILE_TARGET} -march=${_MARCH} -mabi=${_MABI} -Wno-unused-command-line-argument -x assembler-with-cpp")

