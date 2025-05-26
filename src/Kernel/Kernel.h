#pragma once

#include "String.h"

void KernelPanicCore(ReadOnlySpanChar file, uint32_t line, ReadOnlySpanChar message, ...);
#define KernelPanic(message, ...) KernelPanicCore(String(__FILE__), __LINE__, message, __VA_ARGS__)
