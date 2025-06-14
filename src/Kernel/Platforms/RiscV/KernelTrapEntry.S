#include "AsmCommon.h"

.global kernel_trap_entry # TODO: Rename

.section .text.interrupt

kernel_trap_entry: 
    # TODO: We will need to do a switch later when we manage MMU
    # to give the trap handler a kernel space stack independant of user mode.
    # For now, we just save the current SP that is shared between kernel and user.
    csrw  sscratch, sp

    # TODO: For now we are saving all the Gprs but it should be revised later
    # to optimize
    addi  sp, sp, -(36 * PTR_SIZE)
    save_general_purpose_registers sp

    mv    t0, sp
    addi  t0, t0, 31 * PTR_SIZE 
    save_supervisor_registers t0

    la    t1, globalCpuTrapHandler
    load_pointer  t1, (t1)

    mv    a0, sp
    jalr  t1 

    load_general_purpose_registers sp

    mv    t0, sp
    addi  t0, t0, 31 * PTR_SIZE 
    load_supervisor_registers t0

    addi  sp, sp, (36 * PTR_SIZE)

    # TODO: When we will have a dedicated kernel stack per HART,
    # We need to make sure to revert SP before swapping it back
    csrr  sp, sscratch
    
    sret
