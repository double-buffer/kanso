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

// TODO: Replace that with a memory arena
void StringSplit(SpanString* result, ReadOnlySpanChar value, char separator);

// TODO: Replace that with a memory arena
void StringFormat(SpanChar* destination, ReadOnlySpanChar message, ...);
void StringFormatVargs(SpanChar* destination, ReadOnlySpanChar message, va_list vargs);


// TODO: Move that to the standard library
size_t strlen(const char* string);
