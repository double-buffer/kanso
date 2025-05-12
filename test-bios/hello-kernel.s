.global boot

.section .text.kernel

boot:	
    la sp, _STACK_PTR
    call KernelMain
    j .
