#pragma once

//---------------------------------------------------------------------------------------
// Primitive types
//---------------------------------------------------------------------------------------

typedef __UINT8_TYPE__ uint8_t;
typedef __UINT16_TYPE__ uint16_t;
typedef __UINT32_TYPE__ uint32_t;
typedef __UINT64_TYPE__ uint64_t;

typedef __INT8_TYPE__ int8_t;
typedef __INT16_TYPE__ int16_t;
typedef __INT32_TYPE__ int32_t;
typedef __INT64_TYPE__ int64_t;

typedef __UINTPTR_TYPE__ uintptr_t;
typedef __INTPTR_TYPE__ intptr_t; 
typedef __SIZE_TYPE__ size_t;

#define PLATFORM_ARCHITECTURE_BITS (__SIZEOF_POINTER__ * 8)
#define BITS_PER_BYTE 8u
#define BITS_PER_SIZE_TYPE (sizeof(size_t) * BITS_PER_BYTE)
#define MASK_SIZE_TYPE (BITS_PER_SIZE_TYPE - 1)
#define SIZE_MAX __SIZE_MAX__

#define AlignUp(value, align) __builtin_align_up(value, align)
#define IsAligned(value, align) __builtin_is_aligned(value, align)
#define OffsetOf(type, member) __builtin_offsetof(type, member)
#define DivRoundUp(value, divisor) (((value) + (divisor) - 1) / (divisor))

#if __SIZEOF_SIZE_T__ == 8
    #define SizePrefixCountZeros(value) __builtin_ctzll((uint64_t)(value))
#elif __SIZEOF_SIZE_T__ == 4
    #define SizePrefixCountZeros(value) __builtin_ctz((uint32_t)(value))
#endif

//---------------------------------------------------------------------------------------
// Variable parameters
//---------------------------------------------------------------------------------------

#define va_list __builtin_va_list
#define va_start __builtin_va_start
#define va_end __builtin_va_end
#define va_arg __builtin_va_arg

//---------------------------------------------------------------------------------------
// Error Handling
//---------------------------------------------------------------------------------------

typedef enum 
{
    TypeError_None,
    TypeError_InvalidParameter,
    TypeError_NotFound
} TypeError;

// TODO: This will need to be thread local
extern TypeError globalTypeError;

static inline TypeError TypeGetLastError()
{
    return globalTypeError;
}

//---------------------------------------------------------------------------------------
// Endianness conversion
//---------------------------------------------------------------------------------------

typedef enum
{
    ByteOrder_LittleEndian = __ORDER_LITTLE_ENDIAN__,
    ByteOrder_BigEndian = __ORDER_BIG_ENDIAN__ 
} ByteOrder;

#define PLATFORM_BYTE_ORDER __BYTE_ORDER__

// TODO: Read functions for endian

//---------------------------------------------------------------------------------------
// Span
//---------------------------------------------------------------------------------------

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
    } \
    \
    static inline Span##name _SPAN_CAST_##name(size_t sourceStride, void* sourcePointer, size_t sourceLength, const type* unused) \
    { \
        (void)unused; \
        size_t bytes = sourceStride * sourceLength; \
        return CreateSpan##name((type *)sourcePointer, bytes / sizeof(type)); \
    }

#define DefineSpanStackAlloc(name, type, length) \
    (__extension__ ({ \
        static_assert((length) >= 0, "StackAlloc: length must be an integer-constant expression"); \
        type array[(length)]; \
        CreateSpan##name(array, (size_t)(length)); \
    }))

#define DefineSpanCast(name, type, sourceSpan) \
    _SPAN_CAST_##name(sizeof(*(sourceSpan).Pointer), \
               (sourceSpan).Pointer, \
               (sourceSpan).Length, \
               (type*)nullptr)

DefineSpan(Char, char)
#define StackAllocChar(length) DefineSpanStackAlloc(Char, char, (length))

DefineSpan(Uint8, uint8_t)
#define StackAllocUint8(length) DefineSpanStackAlloc(Uint8, uint8_t, (length))

DefineSpan(Uint32, uint32_t)
#define StackAllocUint32(length) DefineSpanStackAlloc(Uint32, uint32_t, (length))
#define SpanCastUint32(sourceSpan) DefineSpanCast(Uint32, uint32_t, (sourceSpan))

DefineSpan(Uint64, uint64_t)
#define StackAllocUint64(length) DefineSpanStackAlloc(Uint64, uint64_t, (length))

DefineSpan(Size, size_t)
#define StackAllocSize(length) DefineSpanStackAlloc(Size, size_t, (length))
#define SpanCastSize(sourceSpan) DefineSpanCast(Size, size_t, (sourceSpan))

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

//---------------------------------------------------------------------------------------
// BitArray
//---------------------------------------------------------------------------------------

typedef struct
{
    SpanSize Data;
    size_t BitCount;
} BitArray;

#define BIT_ARRAY_EMPTY ((BitArray){ .Data = { .Pointer = nullptr, .Length = 0 }, .BitCount = 0 })

static inline bool BitArrayIsEmpty(BitArray bitArray)
{
    return bitArray.Data.Pointer == nullptr;
}

static inline BitArray CreateBitArray(SpanSize data)
{
    globalTypeError = TypeError_None;

    return (BitArray)
    {
        .Data = data,
        .BitCount = data.Length * BITS_PER_SIZE_TYPE
    };
}

static inline BitArray CreateBitArrayWithBitCount(SpanSize data, size_t bitCount)
{
    globalTypeError = TypeError_None;

    return (BitArray)
    {
        .Data = data,
        .BitCount = bitCount
    };
}

static inline bool BitArraySet(BitArray bitArray, size_t index)
{
    if (index >= bitArray.BitCount || BitArrayIsEmpty(bitArray)) 
    {
        globalTypeError = TypeError_InvalidParameter;
        return false;
    }

    bitArray.Data.Pointer[index / BITS_PER_SIZE_TYPE] |= (size_t)1 << (index & MASK_SIZE_TYPE);

    globalTypeError = TypeError_None;
    return true;
}

static inline bool BitArrayReset(BitArray bitArray, size_t index)
{
    if (index >= bitArray.BitCount || BitArrayIsEmpty(bitArray)) 
    {
        globalTypeError = TypeError_InvalidParameter;
        return false;
    }

    bitArray.Data.Pointer[index / BITS_PER_SIZE_TYPE] &= ~((size_t)1 << (index & MASK_SIZE_TYPE));

    globalTypeError = TypeError_None;
    return true;
}

static inline bool BitArrayIsSet(BitArray bitArray, size_t index)
{
    if (index >= bitArray.BitCount || BitArrayIsEmpty(bitArray)) 
    {
        globalTypeError = TypeError_InvalidParameter;
        return false;
    }

    globalTypeError = TypeError_None;

    auto pointer = bitArray.Data.Pointer;
    return ((pointer[index / BITS_PER_SIZE_TYPE] >> (index & MASK_SIZE_TYPE)) & 1) == 1;
}

size_t BitArrayFindFirstNotSet(BitArray bitArray);
size_t BitArrayFindRangeNotSet(BitArray bitArray, size_t length);

//---------------------------------------------------------------------------------------
// Standard types
//---------------------------------------------------------------------------------------

typedef struct
{
    uint8_t Red;
    uint8_t Green;
    uint8_t Blue;
    uint8_t Alpha;
} Color;
