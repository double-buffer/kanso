#pragma once

#include "Types.h"

// TODO: Write tests
// TODO: ReadOnly span char
void StringFormat(char* output, uint32_t* length, const char* message, ...);
void StringFormatVA(char* output, uint32_t* length, const char* message, va_list vargs);

