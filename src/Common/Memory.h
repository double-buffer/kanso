#pragma once

#include "Types.h"

#define DefineSpan(name, T) \
  typedef struct name { \
    T *Pointer; \
    size_t Length; \
  } name

#define Span(pointer, length) \
( \
    (struct { typeof(*(pointer))* Pointer; size_t Length; }) \
    { .Pointer = (pointer), .Length = (length) } \
)

DefineSpan(Uint8Span, uint8_t);
DefineSpan(Uint32Span, uint32_t);
DefineSpan(Uint64Span, uint64_t);

#define MemorySet(span, value) \
    _Generic((span).Pointer, \
      uint8_t*: MemorySetUint8, \
      default:  MemorySetDefault \
    )( \
      (void*)((span).Pointer), \
      (span).Length, \
      sizeof(*((span).Pointer)), \
      &(typeof(*((span).Pointer))){ (value) } \
    )

void MemorySetUint8(void* destination, size_t length, size_t stride, const void* value);
void MemorySetDefault(void* destination, size_t length, size_t stride, const void* value);

// TODO: Span and memory arena

// TODO: Write tests

// TODO: This one is a really good test
/*
    uint8_t test[10];
    auto testSpan = Span(test, 10);
    MemorySet(testSpan, 28);

    KernelConsolePrint("Test: ");

    for (uint32_t i = 0; i < 10; i++)
    {
        KernelConsolePrint("%d: %d, ", i, test[i]);
    }

    KernelConsolePrint("\n");
*/


// TODO: Move that to the standard library
void memset(uint8_t* destination, uint8_t value, size_t sizeInBytes); 
