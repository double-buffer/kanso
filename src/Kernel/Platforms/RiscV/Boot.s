.global boot

.section .text.kernel

boot:
    la   sp, _STACK_PTR

    la   t0, __BSS
    la   t1, __BSS_END
    beq  t0, t1, enter_kernel_main

clear_bss:
    sb   zero, (t0)
    addi t0, t0, 1
    blt  t0, t1, clear_bss

enter_kernel_main:
    call KernelMain
    j    .
