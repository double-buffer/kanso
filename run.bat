@echo off

REM QEMU file path

SET PLATFORM=%1

IF "%2"=="test" (
    SET KERNEL="build/%PLATFORM%/bin/kernel-test.elf"
) ELSE (
    SET KERNEL="build/%PLATFORM%/bin/kernel.elf"
)

SET QEMU=qemu-system-%PLATFORM%

REM Start QEMU
%QEMU% -machine virt -kernel %KERNEL% -serial stdio -monitor none -nographic -no-reboot 
