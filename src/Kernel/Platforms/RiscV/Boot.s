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

.Lrun_init_array:
    # Run init array 
    la    t2, __INIT_ARRAY_START
    la    t3, __INIT_ARRAY_END
    bge   t2, t3, .Lenter_kernel_main

.Lrun_init_array_loop:
    ld    t4, (t2)
    jalr  ra, t4, 0
    addi  t2, t2, 8
    blt   t2, t3, .Lrun_init_array_loop

.Lenter_kernel_main:
    call  KernelMain
    j     .
