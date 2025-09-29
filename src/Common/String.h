#pragma once

#include "Types.h"
#include "Memory.h"

// TODO: Move this header to a standard one? Not a header just for strings?

DefineSpan(String, ReadOnlySpanChar)

static inline ReadOnlySpanChar String(const char* string)
{
    return CreateReadOnlySpan(char, string, __builtin_strlen(string));
}

bool StringEquals(ReadOnlySpanChar string1, ReadOnlySpanChar string2);

ReadOnlySpanString StringSplit(MemoryArena memoryArena, ReadOnlySpanChar value, char separator);

ReadOnlySpanChar StringFormat(MemoryArena memoryArena, ReadOnlySpanChar message, ...);
ReadOnlySpanChar StringFormatVargs(MemoryArena memoryArena, ReadOnlySpanChar message, va_list vargs);


// TODO: Move that to the standard library
size_t strlen(const char* string);
