#!/bin/bash

# QEMU file path
PLATFORM=$1

if [ "$2" = "test" ]; then
    KERNEL="build/$PLATFORM/bin/kernel-test.elf"
else
    KERNEL="build/$PLATFORM/bin/kernel.elf"
fi

QEMU=qemu-system-$PLATFORM

# Start QEMU
$QEMU -machine virt -kernel $KERNEL -serial stdio -monitor none -nographic -no-reboot
