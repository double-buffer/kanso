#pragma once

#include "Types.h"

#define MemoryAlignUp(value, align) __builtin_align_up(value, align)
#define MemoryIsAligned(value, align) __builtin_is_aligned(value, align)
#define MemoryOffsetOf(type, member) __builtin_offsetof(type, member)


#define DEFINE_SPAN(Base, ElemT) \
typedef struct Base##Span { ElemT *Pointer; size_t Length; } Base##Span; \
static inline Base##Span Make##Base##Span(ElemT *ptr, size_t len) { \
    return (Base##Span){ .Pointer = ptr, .Length = len }; \
} \
typedef struct Base##ReadOnlySpan { const ElemT *Pointer; size_t Length; } Base##ReadOnlySpan; \
static inline Base##ReadOnlySpan Make##Base##ReadOnlySpan(const ElemT *ptr, size_t len) { \
    return (Base##ReadOnlySpan){ .Pointer = ptr, .Length = len }; \
} \
static inline Base##ReadOnlySpan To##Base##ReadOnlySpan(Base##Span span) { \
    return (Base##ReadOnlySpan){ .Pointer = span.Pointer, .Length = span.Length }; \
}

DEFINE_SPAN(Uint8,  uint8_t)
DEFINE_SPAN(Uint32, uint32_t)
DEFINE_SPAN(Uint64, uint64_t)

#define Span(ptr, len) \
_Generic((ptr)+0, \
    uint8_t*:  MakeUint8Span, \
    uint32_t*: MakeUint32Span, \
    uint64_t*: MakeUint64Span \
)((ptr), (len))

#define ReadOnlySpan(ptr, len) \
_Generic((ptr)+0, \
    const uint8_t*:  MakeUint8ReadOnlySpan, \
    const uint32_t*: MakeUint32ReadOnlySpan, \
    const uint64_t*: MakeUint64ReadOnlySpan \
)((ptr), (len))

// - helper to build a read-only span -----------------------------------------
#define _RO_CONV(sp, RoType, CastPtr) \
    ((RoType){ .Pointer = (CastPtr)(sp).Pointer, .Length = (sp).Length })

// - 1 if the argument is any of our span structs, else 0 ---------------------
#define _SPAN_OK(x) \
_Generic((x), \
    Uint8Span            : 1, Uint32Span           : 1, Uint64Span           : 1, \
    Uint8ReadOnlySpan    : 1, Uint32ReadOnlySpan   : 1, Uint64ReadOnlySpan   : 1, \
    default              : 0)

// - expression-style, standards-only, compile-time-checked -------------------
#define ToReadOnlySpan(span) \
((void)sizeof(char[ _SPAN_OK(span) ? 1 : -1 ]), /* compile-time guard */ \
 _Generic((span), \
     Uint8ReadOnlySpan  : (span), \
     Uint32ReadOnlySpan : (span), \
     Uint64ReadOnlySpan : (span), \
     Uint8Span  : _RO_CONV(span, Uint8ReadOnlySpan,  const uint8_t* ), \
     Uint32Span : _RO_CONV(span, Uint32ReadOnlySpan, const uint32_t*), \
     Uint64Span : _RO_CONV(span, Uint64ReadOnlySpan, const uint64_t*)  \
 ))



void MemorySetUint8   (void *dst, size_t n, size_t stride, const void *val);
void MemorySetDefault (void *dst, size_t n, size_t stride, const void *val);
void MemoryCopyUint8   (void *dst, size_t n, size_t stride, const void *src, size_t srcLen);
void MemoryCopyDefault (void *dst, size_t n, size_t stride, const void *src, size_t srcLen);

#define MemorySet(span, value) \
_Generic((span).Pointer, \
    uint8_t*: MemorySetUint8, \
    default : MemorySetDefault \
)((span).Pointer, (span).Length, sizeof *(span).Pointer, \
  &(typeof(*(span).Pointer)){ (value) })

#define _MemoryCopyDispatch(DST, SRC) \
_Generic((DST).Pointer, \
    uint8_t*: MemoryCopyUint8, \
    default : MemoryCopyDefault \
)((DST).Pointer, (DST).Length, sizeof *(DST).Pointer, \
  (SRC).Pointer, (SRC).Length)

#define MemoryCopy(dstSpan, srcSpan) \
do { \
    static_assert( \
        _Generic((srcSpan), \
            Uint8ReadOnlySpan  : 1, \
            Uint32ReadOnlySpan : 1, \
            Uint64ReadOnlySpan : 1, \
            default            : 0), \
        "MemoryCopy: source span must be read-only"); \
    _MemoryCopyDispatch((dstSpan), (srcSpan)); \
} while (0)

// TODO: Move that to the standard library
void memset(uint8_t* destination, uint8_t value, size_t sizeInBytes); 
