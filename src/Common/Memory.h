#pragma once

#include "Types.h"

#define MemoryAlignUp(value, align) __builtin_align_up(value, align)
#define MemoryIsAligned(value, align) __builtin_is_aligned(value, align)
#define MemoryOffsetOf(type, member) __builtin_offsetof(type, member)


#define _DefineSpan(prefix, type) \
    typedef struct Span##prefix { type* Pointer; size_t Length; } Span##prefix; \
    typedef struct ReadOnlySpan##prefix { const type* Pointer; size_t Length; } ReadOnlySpan##prefix; \
    \
    static inline Span##prefix MakeSpan##prefix(type* pointer, size_t length) \
    { \
        return (Span##prefix) { .Pointer = pointer, .Length = length }; \
    } \
    \
    static inline ReadOnlySpan##prefix MakeReadOnlySpan##prefix(const type* pointer, size_t length) \
    { \
        return (ReadOnlySpan##prefix) { .Pointer = pointer, .Length = length }; \
    } \
    \
    static inline ReadOnlySpan##prefix ToReadOnlySpan##prefix(Span##prefix span) \
    { \
        return (ReadOnlySpan##prefix) { .Pointer = span.Pointer, .Length = span.Length }; \
    }

_DefineSpan(Char, char)
_DefineSpan(Uint8, uint8_t)
_DefineSpan(Uint32, uint32_t)
_DefineSpan(Uint64, uint64_t)

#define Span(pointer, length) \
    _Generic((pointer)+0, \
        char*: MakeSpanChar, \
        uint8_t*: MakeSpanUint8, \
        uint32_t*: MakeSpanUint32, \
        uint64_t*: MakeSpanUint64 \
    )((pointer), (length))

#define ReadOnlySpan(pointer, length) \
    _Generic((pointer)+0, \
        const char*: MakeReadOnlySpanChar, \
        const uint8_t*: MakeReadOnlySpanUint8, \
        const uint32_t*: MakeReadOnlySpanUint32, \
        const uint64_t*: MakeReadOnlySpanUint64 \
    )((pointer), (length))

#define _ConvertToReadOnlySpan(span, readOnlySpanType, castType) ((readOnlySpanType){ .Pointer = (castType)(span).Pointer, .Length = (span).Length })

#define StackAllocSpan(Type, Count) \
    ( (struct { Type *Pointer; size_t Length; }){    \
          (Type[(Count)]){0}, (size_t)(Count) } )

#define _CheckSpanType(span) \
    _Generic((span), \
        SpanChar: true, \
        SpanUint8: true, \
        SpanUint32: true, \
        SpanUint64: true, \
        ReadOnlySpanChar: true, \
        ReadOnlySpanUint8: true, \
        ReadOnlySpanUint32: true, \
        ReadOnlySpanUint64: true, \
        default: false)

#define ToReadOnlySpan(span) \
( \
    /* Compile time guard */ \
    (void)sizeof(char[ _CheckSpanType(span) ? 1 : -1 ]), \
    _Generic((span), \
        ReadOnlySpanChar: (span), \
        ReadOnlySpanUint8: (span), \
        ReadOnlySpanUint32: (span), \
        ReadOnlySpanUint64: (span), \
        SpanChar: _ConvertToReadOnlySpan(span, ReadOnlySpanChar, const char*), \
        SpanUint8: _ConvertToReadOnlySpan(span, ReadOnlySpanUint8, const uint8_t*), \
        SpanUint32: _ConvertToReadOnlySpan(span, ReadOnlySpanUint32, const uint32_t*), \
        SpanUint64: _ConvertToReadOnlySpan(span, ReadOnlySpanUint64, const uint64_t*) \
    ) \
)

#define SpanSlice(span, offset, length) \
( \
    /* Compile time guard */ \
    (void)sizeof(char[ _CheckSpanType(span) ? 1 : -1 ]), \
    (typeof(span)) \
    { \
        .Pointer = (span).Pointer + (offset), \
        .Length  = (length) \
    } \
)

#define SpanSliceFrom(span, offset) SpanSlice((span), (offset), (span).Length - (offset))

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

#define MemoryCopy(destination, source) \
    do \
    { \
        static_assert( \
            _Generic((source), \
                ReadOnlySpanChar: true, \
                ReadOnlySpanUint8: true, \
                ReadOnlySpanUint32: true, \
                ReadOnlySpanUint64: true, \
                default: false), \
            "MemoryCopy: source span must be read-only"); \
        static_assert(sizeof(*(destination).Pointer) == sizeof(*(source).Pointer), "MemoryCopy: element size mismatch"); \
        _MemoryCopyDispatch((destination), (source)); \
    } while (false)




// TODO: Move that to the standard library
void memset(uint8_t* destination, uint8_t value, size_t sizeInBytes); 
