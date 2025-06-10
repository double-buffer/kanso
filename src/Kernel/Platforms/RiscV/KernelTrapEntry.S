#include "AsmCommon.h"

.global kernel_trap_entry # TODO: Rename

.section .text.interrupt

kernel_trap_entry: 
    # TODO: We will need to do a switch later when we manage MMU
    # to give the trap handler a kernel space stack independant of user mode.
    # For now, we just save the current SP that is shared between kernel and user.
    csrw  sscratch, sp
    csrr  t0, sepc

    li a0, 5

    save_general_purpose_registers
    # TODO: Save supervisor registers


    la    t1, globalCpuTrapHandler
    load_pointer  t1, (t1)

    mv    a0, sp
    jalr  t1 

    load_general_purpose_registers
    # TODO: Load supervisor registers

    csrw  sepc, t0

    # TODO: When we will have a dedicated kernel stack per HART,
    # We need to make sure to revert SP before swapping it back
    csrr  sp, sscratch
    
    sret
