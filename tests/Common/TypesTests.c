#include "Memory.h"
#include "Types.h"
#include "Test.h"

// TODO: SpanSlice: Test Length and overflow
// TODO: SpanCast: Test size compatibility

Test(Types, SpanSlice_WithSpan_HasCorrectValues)
{
    // Arrange
    const uint32_t itemCount = 10;
    const uint32_t sliceOffset = 2;
    const uint32_t sliceLength = 5;

    auto span = StackAllocUint32(itemCount);

    for (uint32_t i = 0; i < itemCount; i++)
    {
        SpanAt(span, i) = i;
    }
    
    // Act
    auto result = SpanSlice(span, sliceOffset, sliceLength);

    // Assert
    TestAssertEquals(sliceLength, result.Length);

    for (uint32_t i = 0; i < result.Length; i++)
    {
        TestAssertEquals(SpanAt(span, i + sliceOffset), SpanAt(result, i));
    }
}

Test(Types, SpanSliceFrom_WithSpan_HasCorrectValues)
{
    // Arrange
    const uint32_t itemCount = 10;
    const uint32_t sliceOffset = 2;

    auto span = StackAllocUint32(itemCount);

    for (uint32_t i = 0; i < itemCount; i++)
    {
        SpanAt(span, i) = i;
    }
    
    // Act
    auto result = SpanSliceFrom(span, sliceOffset);

    // Assert
    TestAssertEquals(itemCount - sliceOffset, result.Length);

    for (uint32_t i = 0; i < result.Length; i++)
    {
        TestAssertEquals(SpanAt(span, i + sliceOffset), SpanAt(result, i));
    }
}

Test(Types, SpanCast_WithSpanUint8ToUint32_HasCorrectValues)
{
    // Arrange
    const uint32_t itemCount = 12;

    auto span = StackAllocUint8(itemCount);

    // Act
    auto result = SpanCast(uint32_t, span);

    // Assert
    TestAssertEquals(itemCount / sizeof(uint32_t), result.Length);
    TestAssertEquals((uintptr_t)span.Pointer, (uintptr_t)result.Pointer);
}

Test(Types, CreateBitArray_WithCorrectBitCount_ReturnsBitArray)
{
    // Arrange
    const uint32_t spanLength = 2;
    const uint32_t bitCount = 13;
    
    auto span = StackAllocSize(spanLength);

    // Act
    // TODO: Convert span
    auto bitArray = CreateBitArray(SpanCast(uint8_t, span), bitCount);

    // Assert
    TestAssertEquals(TypeError_None, TypeGetLastError());

    TestAssertEquals(bitCount, bitArray.BitCount);
    TestAssertEquals(spanLength * sizeof(size_t), bitArray.Data.Length);
    TestAssertEquals((uint8_t*)span.Pointer, bitArray.Data.Pointer);
}

Test(Types, CreateBitArray_WithIncorrectBitCount_ReturnsEmptyBitArray)
{
    // Arrange
    const uint32_t spanLength = 2;
    const uint32_t bitCount = 45;
    
    auto span = StackAllocUint8(spanLength);

    // Act
    auto bitArray = CreateBitArray(span, bitCount);

    // Assert
    TestAssertEquals(TypeError_InvalidParameter, TypeGetLastError());
    TestAssertIsTrue(BitArrayIsEmpty(bitArray));
}

Test(Types, BitArraySet_WithCorrectIndex_HasCorrectValue)
{
    // Arrange
    const uint32_t spanLength = 2;
    const uint32_t bitCount = 13;
    const uint32_t bitIndexToSet = 5;
    
    auto span = StackAllocUint8(spanLength);
    MemorySet(span, 0);

    auto bitArray = CreateBitArray(span, bitCount);

    // Act
    auto result = BitArraySet(bitArray, bitIndexToSet);

    // Assert
    auto testResult = BitArrayIsSet(bitArray, bitIndexToSet);

    TestAssertIsTrue(result);
    TestAssertEquals(TypeError_None, TypeGetLastError());

    TestAssertIsTrue(testResult);
}
