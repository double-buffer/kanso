#pragma once

#include "Types.h"
#include "Memory.h"

ReadOnlySpanChar String(const char* string);

// TODO: Write tests
void StringFormat(char* output, uint32_t* length, const char* message, ...);
void StringFormatVA(char* output, uint32_t* length, const char* message, va_list vargs);

void StringFormat2(SpanChar* destination, ReadOnlySpanChar message, ...);

/*

char destination[2048];
auto destinationSpan = Span(destination);

StringFormat2(destinationSpan, ToReadOnlySpanFromString("This is a test %d\n"), 28);

StringFormat2(destinationSpan, String("This is a test %d\n"), 28);

*/

// TODO: Move that to the standard library
size_t strlen(const char* string);
