#pragma once

//---------------------------------------------------------------------------------------
// Primitive types
//---------------------------------------------------------------------------------------

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
typedef unsigned long long uint64_t;

typedef char int8_t;
typedef short int16_t;
typedef int int32_t;
typedef long long int64_t;

typedef __UINTPTR_TYPE__ uintptr_t;
typedef __INTPTR_TYPE__ intptr_t; 
typedef __SIZE_TYPE__ size_t;

static_assert(sizeof(uint8_t) == 1, "uint8_t must be 1 byte.");
static_assert(sizeof(uint16_t) == 2, "uint16_t must be 2 bytes.");
static_assert(sizeof(uint32_t) == 4,  "uint32_t must be 4 bytes.");
static_assert(sizeof(uint64_t) == 8,  "uint64_t must be 8 bytes.");

static_assert(sizeof(uintptr_t) == sizeof(void *), "uintptr_t is not pointer-sized.");

#define PLATFORM_ARCHITECTURE_BITS (__SIZEOF_POINTER__ * 8)
#define BITS_PER_BYTE 8u
#define BITS_PER_SIZE_TYPE (sizeof(size_t) * BITS_PER_BYTE)
#define MASK_SIZE_TYPE (BITS_PER_SIZE_TYPE - 1)

#define AlignUp(value, align) __builtin_align_up(value, align)
#define IsAligned(value, align) __builtin_is_aligned(value, align)
#define OffsetOf(type, member) __builtin_offsetof(type, member)

// TODO: Cleanup
#if  __SIZEOF_SIZE_T__ == 8
#define CoutTrailingZeros(x)  __builtin_ctzll((unsigned long long)(x))
#elif __SIZEOF_SIZE_T__ == 4
#define CoutTrailingZeros(x)  __builtin_ctz((unsigned int)(x))
#  endif

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
    TypeError_InvalidParameter
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
    [[clang::overloadable]] \
    [[clang::always_inline]] \
    static inline Span##name _SpanCast(size_t sourceStride, void* sourcePointer, size_t sourceLength, const type* unused) \
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

DefineSpan(Char, char)
#define StackAllocChar(length) DefineSpanStackAlloc(Char, char, (length))

DefineSpan(Uint8, uint8_t)
#define StackAllocUint8(length) DefineSpanStackAlloc(Uint8, uint8_t, (length))

DefineSpan(Uint32, uint32_t)
#define StackAllocUint32(length) DefineSpanStackAlloc(Uint32, uint32_t, (length))

DefineSpan(Uint64, uint64_t)
#define StackAllocUint64(length) DefineSpanStackAlloc(Uint64, uint64_t, (length))

DefineSpan(Size, size_t)
#define StackAllocSize(length) DefineSpanStackAlloc(Size, size_t, (length))

#define SpanCast(type, sourceSpan) \
    _SpanCast(sizeof(*(sourceSpan).Pointer), \
               (sourceSpan).Pointer, \
               (sourceSpan).Length, \
               (type*)nullptr)

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
