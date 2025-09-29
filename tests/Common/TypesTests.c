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

    auto span = StackAlloc(uint32_t, itemCount);

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

    auto span = StackAlloc(uint32_t, itemCount);

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

    auto span = StackAlloc(uint8_t, itemCount);

    // Act
    auto result = SpanCast(uint32_t, span);

    // Assert
    TestAssertEquals(itemCount / sizeof(uint32_t), result.Length);
    TestAssertEquals((uintptr_t)span.Pointer, (uintptr_t)result.Pointer);
}

Test(Types, ConvertBytesToUint32_WithBigEndianData_ReturnsCorrectResult)
{
    // Arrange
    uint8_t testData[] = { 0x00, 0x00, 0xB2, 0x73 };

    // Act
    auto result = ConvertBytesToUint32(CreateReadOnlySpan(uint8_t, testData, 4), ByteOrder_BigEndian);

    // Assert
    TestAssertEquals(TypeError_None, TypeGetLastError());
    TestAssertEquals(45683, result);
}

Test(Types, ConvertBytesToUint32_WithLittleEndianData_ReturnsCorrectResult)
{
    // Arrange
    uint8_t testData[] = { 0x73, 0xB2, 0x00, 0x00 };

    // Act
    auto result = ConvertBytesToUint32(CreateReadOnlySpan(uint8_t, testData, 4), ByteOrder_LittleEndian);

    // Assert
    TestAssertEquals(TypeError_None, TypeGetLastError());
    TestAssertEquals(45683, result);
}

Test(Types, ConvertBytesToUint32_WithWrongLength_HasErrorSet)
{
    // Arrange
    uint8_t testData[] = { 0x73, 0xB2 };

    // Act
    auto result = ConvertBytesToUint32(CreateReadOnlySpan(uint8_t, testData, 2), ByteOrder_LittleEndian);

    // Assert
    TestAssertEquals(TypeError_InvalidParameter, TypeGetLastError());
    TestAssertEquals(0, result);
}

Test(Types, ConvertBytesToUint64_WithBigEndianData_ReturnsCorrectResult)
{
    // Arrange
    uint8_t testData[] = { 0x11, 0x22, 0x10, 0xF4, 0x7D, 0xE9, 0x81, 0x15 };

    // Act
    auto result = ConvertBytesToUint64(CreateReadOnlySpan(uint8_t, testData, 8), ByteOrder_BigEndian);

    // Assert
    TestAssertEquals(TypeError_None, TypeGetLastError());
    TestAssertEquals(1234567890123456789ULL, result);
}

Test(Types, ConvertBytesToUint64_WithLittleEndianData_ReturnsCorrectResult)
{
    // Arrange
    uint8_t testData[] = { 0x15, 0x81, 0xE9, 0x7D, 0xF4, 0x10, 0x22, 0x11 };;

    // Act
    auto result = ConvertBytesToUint64(CreateReadOnlySpan(uint8_t, testData, 8), ByteOrder_LittleEndian);

    // Assert
    TestAssertEquals(TypeError_None, TypeGetLastError());
    TestAssertEquals(1234567890123456789ULL, result);
}

Test(Types, ConvertBytesToUint64_WithWrongLength_HasErrorSet)
{
    // Arrange
    uint8_t testData[] = { 0x73, 0xB2 };

    // Act
    auto result = ConvertBytesToUint64(CreateReadOnlySpan(uint8_t, testData, 2), ByteOrder_LittleEndian);

    // Assert
    TestAssertEquals(TypeError_InvalidParameter, TypeGetLastError());
    TestAssertEquals(0, result);
}

Test(Types, CreateBitArray_WithCorrectBitCount_ReturnsBitArray)
{
    // Arrange
    const uint32_t spanLength = 2;
    
    auto span = StackAlloc(size_t, spanLength);

    // Act
    auto bitArray = CreateBitArray(span);

    // Assert
    TestAssertEquals(TypeError_None, TypeGetLastError());

    TestAssertEquals(spanLength, bitArray.Data.Length);
    TestAssertEquals(span.Pointer, bitArray.Data.Pointer);
}

Test(Types, BitArraySet_WithCorrectIndex_HasCorrectValue)
{
    // Arrange
    const uint32_t spanLength = 2;
    const uint32_t bitIndexToSet = 5;
    
    auto span = StackAlloc(size_t, spanLength);
    MemorySet(span, 0);

    auto bitArray = CreateBitArray(span);

    // Act
    auto result = BitArraySet(bitArray, bitIndexToSet);

    // Assert
    TestAssertIsTrue(result);
    TestAssertEquals(TypeError_None, TypeGetLastError());

    auto testResult = BitArrayIsSet(bitArray, bitIndexToSet);
    TestAssertIsTrue(testResult);
}

Test(Types, BitArraySet_WithIncorrectIndex_HasErrorSet)
{
    // Arrange
    const uint32_t spanLength = 2;
    const uint32_t bitIndexToSet = sizeof(size_t) * 8 * 3;
    
    auto span = StackAlloc(size_t, spanLength);
    MemorySet(span, 0);

    auto bitArray = CreateBitArray(span);

    // Act
    auto result = BitArraySet(bitArray, bitIndexToSet);

    // Assert
    TestAssertIsFalse(result);
    TestAssertEquals(TypeError_InvalidParameter, TypeGetLastError());
}

Test(Types, BitArrayReset_WithCorrectIndex_HasCorrectValue)
{
    // Arrange
    const uint32_t spanLength = 2;
    const uint32_t bitIndexToSet = 5;
    
    auto span = StackAlloc(size_t, spanLength);
    MemorySet(span, 1);

    auto bitArray = CreateBitArray(span);

    // Act
    auto result = BitArrayReset(bitArray, bitIndexToSet);

    // Assert
    TestAssertIsTrue(result);
    TestAssertEquals(TypeError_None, TypeGetLastError());

    auto testResult = BitArrayIsSet(bitArray, bitIndexToSet);
    TestAssertIsFalse(testResult);
}

Test(Types, BitArrayReset_WithIncorrectIndex_HasErrorSet)
{
    // Arrange
    const uint32_t spanLength = 2;
    const uint32_t bitIndexToSet = BITS_PER_SIZE_TYPE * 24;
    
    auto span = StackAlloc(size_t, spanLength);
    MemorySet(span, 1);

    auto bitArray = CreateBitArray(span);

    // Act
    auto result = BitArrayReset(bitArray, bitIndexToSet);

    // Assert
    TestAssertIsFalse(result);
    TestAssertEquals(TypeError_InvalidParameter, TypeGetLastError());
}

Test(Types, BitArrayFindFirstNotSet_HasCorrectValue)
{
    // Arrange
    const uint32_t spanLength = 2;
    const uint32_t bitMaxIndexToSet = BITS_PER_SIZE_TYPE+ 3;
    
    auto span = StackAlloc(size_t, spanLength);
    MemorySet(span, 0);

    auto bitArray = CreateBitArray(span);

    for (uint32_t i = 0; i < bitMaxIndexToSet; i++)
    {
        BitArraySet(bitArray, i);
    }

    // Act
    auto result = BitArrayFindFirstNotSet(bitArray);

    // Assert
    TestAssertEquals(bitMaxIndexToSet, result);
}

Test(Types, BitArrayFindRangeNotSet_WithCorrectLength_HasCorrectValue)
{
    // Arrange
    const uint32_t spanLength = 2;
    const uint32_t firstAvailableIndex = BITS_PER_SIZE_TYPE + 3;
    const uint32_t secondAvailableIndex = BITS_PER_SIZE_TYPE + 10;
    const uint32_t gap = 3;
    
    auto span = StackAlloc(size_t, spanLength);
    MemorySet(span, 0);

    auto bitArray = CreateBitArray(span);

    for (uint32_t i = 0; i < firstAvailableIndex; i++)
    {
        BitArraySet(bitArray, i);
    }

    for (uint32_t i = firstAvailableIndex + gap; i < secondAvailableIndex; i++)
    {
        BitArraySet(bitArray, i);
    }

    // Act
    auto result = BitArrayFindRangeNotSet(bitArray, gap + 2);

    // Assert
    TestAssertEquals(secondAvailableIndex, result);
}

Test(Types, BitArrayFindRangeNotSet_WithIncorrectLength_HasErrorSet)
{
    // Arrange
    const uint32_t spanLength = 2;
    const uint32_t incorrectLength = (spanLength * BITS_PER_SIZE_TYPE) + 5;

    auto span = StackAlloc(size_t, spanLength);
    MemorySet(span, 0);

    auto bitArray = CreateBitArray(span);

    // Act
    auto result = BitArrayFindRangeNotSet(bitArray, incorrectLength);

    // Assert
    TestAssertEquals(SIZE_MAX, result);
    TestAssertEquals(TypeError_InvalidParameter, TypeGetLastError());
}
