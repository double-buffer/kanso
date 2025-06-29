#pragma once

#include "Types.h"
#include "Memory.h"

DefineSpan(String, ReadOnlySpanChar)
#define StackAllocString(length) DefineSpanStackAlloc(String, ReadOnlySpanChar, (length))

static inline ReadOnlySpanChar String(const char* string)
{
    return CreateReadOnlySpanChar(string, __builtin_strlen(string));
}

bool StringEquals(ReadOnlySpanChar string1, ReadOnlySpanChar string2);

// TODO: Replace that with a memory arena
void StringSplit(SpanString* result, ReadOnlySpanChar value, char separator);

// TODO: Replace that with a memory arena
void StringFormat(SpanChar* destination, ReadOnlySpanChar message, ...);
void StringFormatVargs(SpanChar* destination, ReadOnlySpanChar message, va_list vargs);


// TODO: Move that to the standard library
size_t strlen(const char* string);
