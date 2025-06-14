#include "Kernel.h"
#include "KernelConsole.h"
#include "Memory.h"
#include "Platform.h"

void KernelFailureCore(ReadOnlySpanChar file, uint32_t line, ReadOnlySpanChar message, ...)
{
    KernelConsoleSetForegroundColor(KernelConsoleColorError);
    KernelConsolePrintBoxMessage(String("Kernel Failure"));
    KernelConsolePrint(String("%s:%d\n"), file, line);

    va_list vargs;
    va_start(vargs, message);

    auto tmp = StackAllocChar(256);
    StringFormatVargs(&tmp, message, vargs);

    KernelConsolePrint(String("%s\n\n"), tmp);
    KernelConsoleResetStyle();

    va_end(vargs);
    
    CpuDisableInterrupts(CpuInterruptType_All);

    while (true) 
    { 
        CpuWaitForInterrupt();
    }
}
