#include "AsmCommon.h"

.global boot

.section .text.kernel

boot:
    # Init the stack pointer
    la    sp, __STACK_PTR

    # Clear .bss
    la    t0, __BSS
    la    t1, __BSS_END
    beq   t0, t1, .Lenter_kernel_main

.Lclear_bss_loop:
    sb    zero, (t0)
    addi  t0, t0, 1
    blt   t0, t1, .Lclear_bss_loop

    # Save boot parameters
    la    t0, globalBootHartId
    save_pointer a0, (t0)

    la    t0, globalDeviceTreeData
    save_pointer a1, (t0)

    # Run init array 
    la    t2, __INIT_ARRAY_START
    la    t3, __INIT_ARRAY_END
    beq   t2, t3, .Lenter_kernel_main

.Lrun_init_array_loop:
    load_pointer  t4, (t2)
    jalr  ra, t4, 0
    addi  t2, t2, PTR_SIZE
    blt   t2, t3, .Lrun_init_array_loop

.Lenter_kernel_main:
    call  KernelMain
    j     .
