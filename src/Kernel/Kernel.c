#include "Kernel.h"
#include "Console.h"
#include "Memory.h"
#include "Platform.h"

void KernelFailureCore(ReadOnlySpanChar file, uint32_t line, ReadOnlySpanChar message, ...)
{
    StackMemoryArena(stackMemoryArena);

    ConsoleSetForegroundColor(ConsoleColorError);
    ConsolePrintBoxMessage(String("Kernel Failure"));
    ConsolePrint(String("%s:%d\n"), file, line);

    va_list vargs;
    va_start(vargs, message);

    auto tmp = StringFormatVargs(stackMemoryArena, message, vargs);

    ConsolePrint(String("%s\n\n"), tmp);
    ConsoleResetStyle();

    va_end(vargs);
    
    CpuDisableInterrupts(CpuInterruptType_All);

    while (true) 
    { 
        CpuWaitForInterrupt();
    }
}
