#include "Console.h"
#include "Memory.h"
#include "String.h"
#include "Platform.h"

void ConsolePrint(ReadOnlySpanChar message, ...)
{
    auto output = StackAlloc(char, 2048);

    va_list vargs;
    va_start(vargs, message);
    
    StringFormatVargs(&output, message, vargs);

    va_end(vargs);
    
    BiosDebugConsoleWrite(ToReadOnlySpan(char, output));
}
