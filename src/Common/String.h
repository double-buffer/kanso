#pragma once

#include "Types.h"

// TODO: ReadOnly span char
void StringFormat(char* output, uint32_t* length, const char* message, ...);
void StringFormatVA(char* output, uint32_t* length, const char* message, __builtin_va_list vargs);

