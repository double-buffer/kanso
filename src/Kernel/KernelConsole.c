#include "String.h"
#include "Platform.h"
#include "KernelConsole.h"

// TODO: See https://github.com/riscv-software-src/opensbi/blob/master/lib/sbi/sbi_console.c#L440 for more implementation details
void KernelConsolePrint(const char* message, ...)
{
    char output[2048] = {};
    uint32_t length = 0;

    __builtin_va_list vargs;
    __builtin_va_start(vargs, message);
    
    StringFormatVA(output, &length, message, vargs);

    __builtin_va_end(vargs);
    
    BiosDebugConsoleWrite(output, length);
}
