#include "KernelConsole.h"
#include "String.h"
#include "Platform.h"

// TODO: See https://github.com/riscv-software-src/opensbi/blob/master/lib/sbi/sbi_console.c#L440 for more implementation details
void KernelConsolePrint(ReadOnlySpanChar message, ...)
{
    auto output = StackAllocChar(2048);

    va_list vargs;
    va_start(vargs, message);
    
    StringFormatVargs(&output, message, vargs);

    va_end(vargs);
    
    BiosDebugConsoleWrite(output.Pointer, output.Length);
}
