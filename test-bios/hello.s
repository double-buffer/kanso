.global boot

.section .text.bios

boot:	
    li a0, 0x10000000
    la a1, message

print_loop:
    lbu a2, (a1)
    beq a2, zero, print_end
    sb a2, (a0)
    addi a1, a1, 1
    j print_loop

print_end:
    j print_end

.section .rodata

message:
    .asciz "Hello Bios!"
