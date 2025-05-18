ENTRY(boot)

MEMORY
{
    kernel_space (rwx) : ORIGIN = 0x80200000, LENGTH = 2 * 1024 * 1024
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

    .data : 
    {
        *(.data);
    } > kernel_space

    .rodata :
    {
        *(.rodata);
    } > kernel_space

    .bss :
    {
        __BSS = .;
        *(.bss .sbss);
        __BSS_END = .;
    } > kernel_space

    . = ALIGN(8);
    . += 128 * 1024; /* 128KB */
    _STACK_PTR = .;
}
