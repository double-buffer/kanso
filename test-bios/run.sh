#!/bin/bash
set -xue

export PATH="$PATH:$(brew --prefix)/opt/llvm/bin"

#clang-20 --target=riscv64-unknown-elf -c hello.s -o hello.o
clang-20 --target=riscv64-unknown-elf -Wall -Wextra -Wall -Wextra -fno-stack-protector -nostdlib -ffreestanding -mcmodel=medany -c hello.c -o hello.o

clang-20 --target=riscv64-unknown-elf -nostdlib -Wl,-Thello.ld -Wl,-Map=hello.map -o hello.elf hello.o

# QEMU file path
QEMU=qemu-system-riscv64

# Start QEMU
$QEMU -machine virt -bios hello.elf -serial mon:stdio -nographic
