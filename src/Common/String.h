#pragma once

#include "Types.h"
#include "Memory.h"

ReadOnlySpanChar String(const char* string);

// TODO: Write tests

void StringFormat(SpanChar* destination, ReadOnlySpanChar message, ...);
void StringFormatVargs(SpanChar* destination, ReadOnlySpanChar message, va_list vargs);

// TODO: Move that to the standard library
size_t strlen(const char* string);
