#!/bin/bash
set -xue

export PATH="$PATH:$(brew --prefix)/opt/llvm/bin"

#clang-20 --target=riscv64-unknown-elf -c hello.s -o hello.o
clang-20 --target=riscv64-unknown-elf -c hello-kernel.s -o hello-kernel-boot.o

#clang-20 --target=riscv64-unknown-elf -Wall -Wextra -Wall -Wextra -fno-stack-protector -nostdlib -ffreestanding -mcmodel=medany -c hello.c -o hello.o
clang-20 --target=riscv64-unknown-elf -Wall -Wextra -Wall -Wextra -fno-stack-protector -nostdlib -ffreestanding -mcmodel=medany -c hello-kernel.c -o hello-kernel.o

#clang-20 --target=riscv64-unknown-elf -nostdlib -Wl,-Thello.ld -Wl,-Map=hello.map -o hello.elf hello.o
clang-20 --target=riscv64-unknown-elf -nostdlib -Wl,-Thello-kernel.ld -Wl,-Map=hello-kernel.map -o hello-kernel.elf hello-kernel-boot.o hello-kernel.o

# QEMU file path
QEMU=qemu-system-riscv64

# Start QEMU
#$QEMU -machine virt -bios hello.elf -serial mon:stdio -nographic
#$QEMU -machine virt -kernel hello-kernel.elf -serial mon:stdio -nographic

llvm-objdump -D ../build/riscv64/bin/kernel.elf
#llvm-objdump -D hello-kernel.elf
