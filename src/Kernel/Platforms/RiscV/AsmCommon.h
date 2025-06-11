#pragma once

#if __riscv_xlen == 64
    .equ PTR_SIZE, 8
    .macro load_pointer rd, addr
        ld \rd, \addr
    .endm
    .macro save_pointer rs, addr
        sd \rs, \addr
    .endm
#else
    .equ PTR_SIZE, 4
    .macro load_pointer rd, addr
        lw \rd, \addr
    .endm
    .macro save_pointer rs, addr
        sw \rs, \addr
    .endm
#endif


.macro save_general_purpose_registers addr
    save_pointer  x1, 0 * PTR_SIZE(\addr)
    save_pointer  x2, 1 * PTR_SIZE(\addr)
    save_pointer  x3, 2 * PTR_SIZE(\addr)
    save_pointer  x4, 3 * PTR_SIZE(\addr)
    save_pointer  x5, 4 * PTR_SIZE(\addr)
    save_pointer  x6, 5 * PTR_SIZE(\addr)
    save_pointer  x7, 6 * PTR_SIZE(\addr)
    save_pointer  x8, 7 * PTR_SIZE(\addr)
    save_pointer  x9, 8 * PTR_SIZE(\addr)
    save_pointer  x10, 9 * PTR_SIZE(\addr)
    save_pointer  x11, 10 * PTR_SIZE(\addr)
    save_pointer  x12, 11 * PTR_SIZE(\addr)
    save_pointer  x13, 12 * PTR_SIZE(\addr)
    save_pointer  x14, 13 * PTR_SIZE(\addr)
    save_pointer  x15, 14 * PTR_SIZE(\addr)
    save_pointer  x16, 15 * PTR_SIZE(\addr)
    save_pointer  x17, 16 * PTR_SIZE(\addr)
    save_pointer  x18, 17 * PTR_SIZE(\addr)
    save_pointer  x19, 18 * PTR_SIZE(\addr)
    save_pointer  x20, 19 * PTR_SIZE(\addr)
    save_pointer  x21, 20 * PTR_SIZE(\addr)
    save_pointer  x22, 21 * PTR_SIZE(\addr)
    save_pointer  x23, 22 * PTR_SIZE(\addr)
    save_pointer  x24, 23 * PTR_SIZE(\addr)
    save_pointer  x25, 24 * PTR_SIZE(\addr)
    save_pointer  x26, 25 * PTR_SIZE(\addr)
    save_pointer  x27, 26 * PTR_SIZE(\addr)
    save_pointer  x28, 27 * PTR_SIZE(\addr)
    save_pointer  x29, 28 * PTR_SIZE(\addr)
    save_pointer  x30, 29 * PTR_SIZE(\addr)
    save_pointer  x31, 30 * PTR_SIZE(\addr)
.endm

.macro load_general_purpose_registers addr
    load_pointer  x1, 0 * PTR_SIZE(\addr)
    load_pointer  x2, 1 * PTR_SIZE(\addr)
    load_pointer  x3, 2 * PTR_SIZE(\addr)
    load_pointer  x4, 3 * PTR_SIZE(\addr)
    load_pointer  x5, 4 * PTR_SIZE(\addr)
    load_pointer  x6, 5 * PTR_SIZE(\addr)
    load_pointer  x7, 6 * PTR_SIZE(\addr)
    load_pointer  x8, 7 * PTR_SIZE(\addr)
    load_pointer  x9, 8 * PTR_SIZE(\addr)
    load_pointer  x10, 9 * PTR_SIZE(\addr)
    load_pointer  x11, 10 * PTR_SIZE(\addr)
    load_pointer  x12, 11 * PTR_SIZE(\addr)
    load_pointer  x13, 12 * PTR_SIZE(\addr)
    load_pointer  x14, 13 * PTR_SIZE(\addr)
    load_pointer  x15, 14 * PTR_SIZE(\addr)
    load_pointer  x16, 15 * PTR_SIZE(\addr)
    load_pointer  x17, 16 * PTR_SIZE(\addr)
    load_pointer  x18, 17 * PTR_SIZE(\addr)
    load_pointer  x19, 18 * PTR_SIZE(\addr)
    load_pointer  x20, 19 * PTR_SIZE(\addr)
    load_pointer  x21, 20 * PTR_SIZE(\addr)
    load_pointer  x22, 21 * PTR_SIZE(\addr)
    load_pointer  x23, 22 * PTR_SIZE(\addr)
    load_pointer  x24, 23 * PTR_SIZE(\addr)
    load_pointer  x25, 24 * PTR_SIZE(\addr)
    load_pointer  x26, 25 * PTR_SIZE(\addr)
    load_pointer  x27, 26 * PTR_SIZE(\addr)
    load_pointer  x28, 27 * PTR_SIZE(\addr)
    load_pointer  x29, 28 * PTR_SIZE(\addr)
    load_pointer  x30, 29 * PTR_SIZE(\addr)
    load_pointer  x31, 30 * PTR_SIZE(\addr)
.endm

.macro save_supervisor_registers addr
    csrr  t1, sepc
    save_pointer  t1, 0 * PTR_SIZE(\addr)
    csrr  t1, sstatus
    save_pointer  t1, 1 * PTR_SIZE(\addr)
    csrr  t1, sscratch 
    save_pointer  t1, 2 * PTR_SIZE(\addr)
    csrr  t1, scause 
    save_pointer  t1, 3 * PTR_SIZE(\addr)
    csrr  t1, stval 
    save_pointer  t1, 4 * PTR_SIZE(\addr)
.endm

.macro load_supervisor_registers addr
    load_pointer  t1, 0 * PTR_SIZE(\addr)
    csrw  sepc, t1
    load_pointer  t1, 1 * PTR_SIZE(\addr)
    csrw  sstatus, t1
    load_pointer  t1, 2 * PTR_SIZE(\addr)
    csrw  sscratch, t1 
    load_pointer  t1, 3 * PTR_SIZE(\addr)
    csrw  scause, t1 
    load_pointer  t1, 4 * PTR_SIZE(\addr)
    csrw  stval, t1 
.endm
