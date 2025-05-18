@echo off

REM QEMU file path
SET QEMU=qemu-system-riscv64

IF "%1"=="test" (
    SET KERNEL=build/riscv64/bin/kernel-test.elf
) ELSE (
    SET KERNEL=build/riscv64/bin/kernel.elf
)

REM Start QEMU
%QEMU% -machine virt -kernel %KERNEL% -serial stdio -monitor none -nographic -no-reboot 
