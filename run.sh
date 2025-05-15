#!/bin/bash
set -xue

# QEMU file path
QEMU=qemu-system-riscv64

# Start QEMU
$QEMU -machine virt -kernel build/riscv64/bin/kernel.elf -serial mon:stdio -nographic
