#!/bin/bash

# QEMU file path
QEMU=qemu-system-riscv64

if [ "$1" = "test" ]; then
    KERNEL="build/riscv64/bin/kernel-test.elf"
else
    KERNEL="build/riscv64/bin/kernel.elf"
fi

# Start QEMU
$QEMU -machine virt -kernel $KERNEL -serial stdio -monitor none  -nographic -no-reboot
