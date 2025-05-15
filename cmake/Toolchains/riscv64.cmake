set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR riscv64)

execute_process(
  COMMAND brew --prefix llvm
  OUTPUT_VARIABLE BREW_LLVM_PREFIX
  OUTPUT_STRIP_TRAILING_WHITESPACE
)

# TODO: Is it needed?
#list(PREPEND CMAKE_PROGRAM_PATH "${BREW_LLVM_PREFIX}/bin")

# 3) Now find clang-20 (falling back to clang)
find_program(_CLANG
  NAMES clang-20 clang
  HINTS "${BREW_LLVM_PREFIX}/bin"
  NO_CMAKE_SYSTEM_PATH        # ignore /usr/bin
)

if(NOT _CLANG)
  message(FATAL_ERROR "Could not find clang in ${HOMEBREW_LLVM_BIN} or \$PATH")
endif()

message("CLANG: ${_CLANG}")

set(CMAKE_C_COMPILER "${_CLANG}")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Wpedantic --target=riscv64-unknown-elf -fno-stack-protector -nostdlib -ffreestanding -mcmodel=medany")

set(CMAKE_ASM_COMPILER "${_CLANG}")
set(CMAKE_ASM_FLAGS "--target=riscv64-unknown-elf")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

