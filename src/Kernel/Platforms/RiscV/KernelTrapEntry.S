#include "AsmCommon.h"

.global supervisor_trap_entry # TODO: Rename

.section .text.interrupt

supervisor_trap_entry: 
    # TODO: We will need to do a switch later when we manage MMU
    # to give the trap handler a kernel space stack independant of user mode.
    # For now, we just save the current SP that is shared between kernel and user.
    csrw  sscratch, sp
    csrr  t0, sepc

    save_general_purpose_registers
    # TODO: Save supervisor registers

    mv    a0, sp # TODO: I don't know why we should have this one

    la    t1, globalCpuTrapHandler
    load_pointer  t1, (t1)
    jalr  t1 

    load_general_purpose_registers
    # TODO: Load supervisor registers

    csrw  sepc, t0

    # TODO: When we will have a dedicated kernel stack per HART,
    # We need to make sure to revert SP before swapping it back
    csrr  sp, sscratch
    
    sret
