ENTRY(boot)

SECTIONS 
{
    . = 0x80200000;

    .text :
    {
        *(.text.kernel) 
        *(.text);
    }

    .text.interrupt : ALIGN(4) 
    {
        *(.text.interrupt .text.interrupt.*)
    }

    .data : ALIGN(8)
    {
        *(.data);
    } 

    .rodata : ALIGN(8) 
    {
        *(.rodata);
    }

    .init_array ALIGN(8) :
    {
        __INIT_ARRAY_START = .;
        KEEP(*(SORT_BY_INIT_PRIORITY(.init_array.*)))
        KEEP(*(.init_array))
        __INIT_ARRAY_END = .;
    }

    .bss (NOLOAD) : ALIGN(8) 
    {
        __BSS = .;
        *(.bss .bss.* .sbss .sbss.*)
        *(COMMON)
        __BSS_END = .;
    }

    . = ALIGN(8);
    . += 128 * 1024; /* 128KB */
    __STACK_PTR = .;

    . = ALIGN(4096);
    __INIT_HEAP_START = .;
    . += 64 * 1024 * 1024; /* 64MB, TODO: Review this later, we don't need that much for init mode */
    __INIT_HEAP_END = .;
}
