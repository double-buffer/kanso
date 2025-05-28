#pragma once

#if __riscv_xlen == 64
    .equ PTR_SIZE, 8
    .macro LOAD_PTR rd, addr
        ld \rd, \addr
    .endm
    .macro STORE_PTR rs, addr
        sd \rs, \addr
    .endm
#else
    .equ PTR_SIZE, 4
    .macro LOAD_PTR rd, addr
        lw \rd, \addr
    .endm
    .macro STORE_PTR rs, addr
        sw \rs, \addr
    .endm
#endif


