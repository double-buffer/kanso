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

#define MemoryAlignUp(value, align)   __builtin_align_up(value, align)
#define MemoryIsAligned(value, align) __builtin_is_aligned(value, align)
#define MemoryOffsetOf(type, member)   __builtin_offsetof(type, member)

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

#define MemoryCopy(destinationSpan, sourceSpan) \
    _Generic((destinationSpan).Pointer, \
      uint8_t*: MemoryCopyUint8, \
      default:  MemoryCopyDefault \
    )( \
      (void*)((destinationSpan).Pointer), \
      (destinationSpan).Length, \
      sizeof(*((destinationSpan).Pointer)), \
      (void*)((sourceSpan).Pointer), \
      (sourceSpan).Length \
    )

void MemoryCopyUint8(void* destination, size_t destinationLength, size_t stride, const void* source, size_t sourceLength);
void MemoryCopyDefault(void* destination, size_t destinationLength, size_t stride, const void* source, size_t sourceLength);

// TODO: Move that to the standard library
void memset(uint8_t* destination, uint8_t value, size_t sizeInBytes); 
