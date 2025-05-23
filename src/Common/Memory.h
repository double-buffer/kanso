#pragma once

#include "Types.h"

#define MemoryAlignUp(value, align) __builtin_align_up(value, align)
#define MemoryIsAligned(value, align) __builtin_is_aligned(value, align)
#define MemoryOffsetOf(type, member) __builtin_offsetof(type, member)


#define _DefineSpan(prefix, type) \
    typedef struct prefix##Span { type *Pointer; size_t Length; } prefix##Span; \
    typedef struct prefix##ReadOnlySpan { const type *Pointer; size_t Length; } prefix##ReadOnlySpan; \
    \
    static inline prefix##Span Make##prefix##Span(type *ptr, size_t len) \
    { \
        return (prefix##Span) { .Pointer = ptr, .Length = len }; \
    } \
    \
    static inline prefix##ReadOnlySpan Make##prefix##ReadOnlySpan(const type *ptr, size_t len) \
    { \
        return (prefix##ReadOnlySpan) { .Pointer = ptr, .Length = len }; \
    } \
    \
    static inline prefix##ReadOnlySpan To##prefix##ReadOnlySpan(prefix##Span span) \
    { \
        return (prefix##ReadOnlySpan) { .Pointer = span.Pointer, .Length = span.Length }; \
    }

_DefineSpan(Uint8,  uint8_t)
_DefineSpan(Uint32, uint32_t)
_DefineSpan(Uint64, uint64_t)

#define Span(pointer, length) \
    _Generic((pointer)+0, \
        uint8_t*: MakeUint8Span, \
        uint32_t*: MakeUint32Span, \
        uint64_t*: MakeUint64Span \
    )((pointer), (length))

#define ReadOnlySpan(pointer, length) \
    _Generic((pointer)+0, \
        const uint8_t*: MakeUint8ReadOnlySpan, \
        const uint32_t*: MakeUint32ReadOnlySpan, \
        const uint64_t*: MakeUint64ReadOnlySpan \
    )((pointer), (length))

#define _ConvertToReadOnlySpan(span, readOnlySpanType, castType) ((readOnlySpanType){ .Pointer = (castType)(span).Pointer, .Length = (span).Length })

#define _CheckSpanType(span) \
    _Generic((span), \
        Uint8Span: true, \
        Uint32Span: true, \
        Uint64Span: true, \
        Uint8ReadOnlySpan: true, \
        Uint32ReadOnlySpan: true, \
        Uint64ReadOnlySpan: true, \
        default: false)

#define ToReadOnlySpan(span) \
( \
    /* Compile time guard */ \
    (void)sizeof(char[ _CheckSpanType(span) ? 1 : -1 ]), \
    _Generic((span), \
        Uint8ReadOnlySpan: (span), \
        Uint32ReadOnlySpan: (span), \
        Uint64ReadOnlySpan: (span), \
        Uint8Span: _ConvertToReadOnlySpan(span, Uint8ReadOnlySpan, const uint8_t*), \
        Uint32Span: _ConvertToReadOnlySpan(span, Uint32ReadOnlySpan, const uint32_t*), \
        Uint64Span: _ConvertToReadOnlySpan(span, Uint64ReadOnlySpan, const uint64_t*) \
    ) \
)



void MemorySetUint8(size_t stride, void* destination, size_t destinationLength, const void* value);
void MemorySetDefault(size_t stride, void* destination, size_t destinationLength, const void* value);

#define MemorySet(destination, value) \
    _Generic((destination).Pointer, \
        uint8_t*: MemorySetUint8, \
        default: MemorySetDefault \
    )(sizeof(*(destination).Pointer), (destination).Pointer, (destination).Length, &(typeof(*(destination).Pointer)){ (value) })


void MemoryCopyUint8(size_t stride, void* destination, size_t destinationLength, const void* source, size_t sourceLength);
void MemoryCopyDefault(size_t stride, void* destination, size_t destinationLength, const void* source, size_t sourceLength);

#define _MemoryCopyDispatch(destination, source) \
    _Generic((destination).Pointer, \
        uint8_t*: MemoryCopyUint8, \
        default: MemoryCopyDefault \
    )(sizeof(*(destination).Pointer), (destination).Pointer, (destination).Length, (source).Pointer, (source).Length)

#define MemoryCopy(destination, source) \
    do \
    { \
        static_assert( \
            _Generic((source), \
                Uint8ReadOnlySpan  : 1, \
                Uint32ReadOnlySpan : 1, \
                Uint64ReadOnlySpan : 1, \
                default            : 0), \
            "MemoryCopy: source span must be read-only"); \
        _MemoryCopyDispatch((destination), (source)); \
    } while (false)




// TODO: Move that to the standard library
void memset(uint8_t* destination, uint8_t value, size_t sizeInBytes); 
