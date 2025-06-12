#include "Kernel.h"
#include "KernelConsole.h"
#include "Platform.h"

void KernelFailureCore(ReadOnlySpanChar file, uint32_t line, ReadOnlySpanChar message, ...)
{
    KernelConsolePrint(String("\x1b[31m\n\xDA\xC4\xC4\xC4----------------\xBF\n"));
    KernelConsolePrint(String("\xB3 KERNEL Failure |\n"));
    KernelConsolePrint(String("\xC0----------------\xD9\n\n"));
    KernelConsolePrint(String("%s:%d\n"), file, line);

    va_list vargs;
    va_start(vargs, message);

    auto tmp = StackAllocChar(256);
    StringFormatVargs(&tmp, message, vargs);

    KernelConsolePrint(String("%s\n\n\x1b[0m"), tmp);

    va_end(vargs);
    
    CpuDisableInterrupts(CpuInterruptType_All);

    while (true) 
    { 
        CpuWaitForInterrupt();
    }
}
