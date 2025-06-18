#pragma once

#include "Types.h"

#define MemoryAlignUp(value, align) __builtin_align_up(value, align)
#define MemoryIsAligned(value, align) __builtin_is_aligned(value, align)
#define MemoryOffsetOf(type, member) __builtin_offsetof(type, member)


#define DefineSpan(name, type) \
    typedef struct Span##name { type* Pointer; size_t Length; } Span##name; \
    typedef struct ReadOnlySpan##name { const type* Pointer; size_t Length; } ReadOnlySpan##name; \
    \
    static inline Span##name CreateSpan##name(type* pointer, size_t length) \
    { \
        return (Span##name) { .Pointer = pointer, .Length = length }; \
    } \
    \
    static inline ReadOnlySpan##name CreateReadOnlySpan##name(const type* pointer, size_t length) \
    { \
        return (ReadOnlySpan##name) { .Pointer = pointer, .Length = length }; \
    } \
    \
    static inline ReadOnlySpan##name ToReadOnlySpan##name(Span##name span) \
    { \
        return (ReadOnlySpan##name) { .Pointer = span.Pointer, .Length = span.Length }; \
    }

#define DefineSpanStackAlloc(name, type, length) \
    (__extension__ ({ \
        static_assert((length) >= 0, "StackAlloc: length must be an integer-constant expression"); \
        type array[(length)]; \
        CreateSpan##name(array, (size_t)(length)); \
    }))

DefineSpan(Char, char)
#define StackAllocChar(length) DefineSpanStackAlloc(Char, char, (length))

DefineSpan(Uint8, uint8_t)
#define StackAllocUint8(length) DefineSpanStackAlloc(Uint8, uint8_t, (length))

DefineSpan(Uint32, uint32_t)
#define StackAllocUint32(length) DefineSpanStackAlloc(Uint32, uint32_t, (length))

DefineSpan(Uint64, uint64_t)
#define StackAllocUint64(length) DefineSpanStackAlloc(Uint64, uint64_t, (length))

#define SpanSlice(span, offset, length) \
( \
    (typeof(span)) \
    { \
        .Pointer = (span).Pointer + (offset), \
        .Length  = (length) \
    } \
)

#define SpanSliceFrom(span, offset) SpanSlice((span), (offset), (span).Length - (offset))
#define SpanAt(span, index) (span).Pointer[(index)]

void MemorySetByte(size_t stride, void* destination, size_t destinationLength, const void* value);
void MemorySetDefault(size_t stride, void* destination, size_t destinationLength, const void* value);

#define MemorySet(destination, value) \
    _Generic((destination).Pointer, \
        char*: MemorySetByte, \
        uint8_t*: MemorySetByte, \
        default: MemorySetDefault \
    )(sizeof(*(destination).Pointer), (destination).Pointer, (destination).Length, &(typeof(*(destination).Pointer)){ (value) })


void MemoryCopyByte(size_t stride, void* destination, size_t destinationLength, const void* source, size_t sourceLength);
void MemoryCopyDefault(size_t stride, void* destination, size_t destinationLength, const void* source, size_t sourceLength);

#define _MemoryCopyDispatch(destination, source) \
    _Generic((destination).Pointer, \
        char*: MemoryCopyByte, \
        uint8_t*: MemoryCopyByte, \
        default: MemoryCopyDefault \
    )(sizeof(*(destination).Pointer), (destination).Pointer, (destination).Length, (source).Pointer, (source).Length)


#define _IS_READONLY_SPAN(span)                                            \
        __builtin_types_compatible_p(                                      \
                __typeof__((span).Pointer),                                \
                const __typeof__(*(span).Pointer) *)

#define _ASSERT_READONLY_SPAN(src)                                         \
        _Static_assert( _IS_READONLY_SPAN(src),                            \
                        "MemoryCopy: source span must be read-only")

#define MemoryCopy(destination, source)                                           \
    do {                                                                          \
        _ASSERT_READONLY_SPAN(source);         \
        _MemoryCopyDispatch((destination), (source));                             \
    } while (0)



// TODO: Move that to the standard library
void memset(uint8_t* destination, uint8_t value, size_t sizeInBytes); 
