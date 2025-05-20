ENTRY(boot)

MEMORY
{
    kernel_space (rwx) : ORIGIN = 0x80200000, LENGTH = 64 * 1024 * 1024
}

SECTIONS 
{
    .text :
    {
        *(.text.kernel) 
        *(.text);
    } > kernel_space

    .text.interrupt : ALIGN(4) 
    {
    }

    .data : ALIGN(8)
    {
        *(.data);
    } > kernel_space

    .rodata : ALIGN(8) 
    {
        *(.rodata);
    } > kernel_space

    .init_array ALIGN(8) :
    {
        __INIT_ARRAY_START = .;
        KEEP(*(SORT_BY_INIT_PRIORITY(.init_array.*)))
        KEEP(*(.init_array))
        __INIT_ARRAY_END = .;
    } > kernel_space

    .bss : ALIGN(8) 
    {
        __BSS = .;
        *(.bss .sbss);
        __BSS_END = .;
    } > kernel_space

    . = ALIGN(8);
    . += 128 * 1024; /* 128KB */
    __STACK_PTR = .;
}
