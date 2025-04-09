typedef unsigned char uint8_t;
typedef unsigned int uint32_t;
typedef unsigned long uint64_t;
typedef uint64_t size_t;

extern char __bss[], __bss_end[], __stack_top[];


__attribute__((naked))
void boot()
{
    __asm__ __volatile__(
        "mv sp, %[stack_top]\n" // Set the stack pointer
        "j startup\n"       // Jump to the kernel main function
        :
        : [stack_top] "r" (__stack_top) // Pass the stack top address as %[stack_top]
    );
}

void *memset(void *buf, char c, size_t n) 
{
    uint8_t *p = (uint8_t *) buf;
    while (n--)
        *p++ = c;
    return buf;
}

void kernel_main(void) 
{
    memset(__bss, 0, (size_t) __bss_end - (size_t) __bss);

    for (;;);
}

void startup()
{
    volatile uint8_t* destination = (volatile uint8_t*)0x10000000;
    char* stringValue = "Hello Bios from C!\n";

    char* pointer = stringValue;

    while (*pointer != '\0')
    {
        *destination = *pointer;
        pointer++;
    }

    while(1);
}
