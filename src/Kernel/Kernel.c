#include "Kernel.h"
#include "KernelConsole.h"

void KernelPanicCore(ReadOnlySpanChar file, uint32_t line, ReadOnlySpanChar message, ...)
{
    KernelConsolePrint(String("\n================\n"));
    KernelConsolePrint(String("| KERNEL PANIC |\n"));
    KernelConsolePrint(String("================\n\n"));
    KernelConsolePrint(String("%s:%d\n"), file, line);

    va_list vargs;
    va_start(vargs, message);

    auto tmp = StackAllocChar(256);
    StringFormatVargs(&tmp, message, vargs);

    KernelConsolePrint(String("%s\n"), tmp);

    va_end(vargs);
    
    // TODO: Disable interrupts and WFI
    while (true) {} 
}
