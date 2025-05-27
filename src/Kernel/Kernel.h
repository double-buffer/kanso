#pragma once

#include "String.h"

void KernelFailureCore(ReadOnlySpanChar file, uint32_t line, ReadOnlySpanChar message, ...);
#define KernelFailure(message, ...) KernelFailureCore(String(__FILE__), __LINE__, message, __VA_ARGS__)
