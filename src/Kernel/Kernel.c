#include "Kernel.h"
#include "KernelConsole.h"
#include "Platform.h"

void KernelFailureCore(ReadOnlySpanChar file, uint32_t line, ReadOnlySpanChar message, ...)
{
    KernelConsolePrint(String("\x1b[31m\n ----------------\n"));
    KernelConsolePrint(String("| KERNEL Failure |\n"));
    KernelConsolePrint(String(" ----------------\n\n"));
    KernelConsolePrint(String("%s:%d\n"), file, line);

    va_list vargs;
    va_start(vargs, message);

    auto tmp = StackAllocChar(256);
    StringFormatVargs(&tmp, message, vargs);

    KernelConsolePrint(String("%s\n\n\x1b[0m"), tmp);

    va_end(vargs);
    
    CpuDisableSupervisorInterrupts(CpuInterruptType_All);

    while (true) 
    { 
        CpuWaitForInterrupt();
    }
}
