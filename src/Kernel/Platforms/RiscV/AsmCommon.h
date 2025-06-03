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


