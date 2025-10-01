#include "Kanso.h"
#include "Platform.h"

void ConsolePrint(ReadOnlySpanChar message, ...)
{
    StackMemoryArena(stackMemoryArena);

    va_list vargs;
    va_start(vargs, message);
    
    auto output = StringFormatVargs(stackMemoryArena, message, vargs);

    va_end(vargs);
    
    BiosDebugConsoleWrite(output);
}
