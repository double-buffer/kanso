#pragma once

#include "Types.h"
#include "Memory.h"

ReadOnlySpanChar String(const char* string);
bool StringEquals(ReadOnlySpanChar string1, ReadOnlySpanChar string2);

void StringFormat(SpanChar* destination, ReadOnlySpanChar message, ...);
void StringFormatVargs(SpanChar* destination, ReadOnlySpanChar message, va_list vargs);



// TODO: Move that to the standard library
size_t strlen(const char* string);
