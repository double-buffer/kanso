#include "Test.h"
#include "Memory.h"

// TODO: SpanSlice: Test Length
// TODO: Memory set with a span that has less elements

Test(Memory, SpanSlice_WithSpan_HasCorrectValues)
{
    // Arrange
    const uint32_t itemCount = 10;
    const uint32_t sliceOffset = 2;
    const uint32_t sliceLength = 5;

    auto span = StackAlloc(itemCount);

    for (uint32_t i = 0; i < itemCount; i++)
    {
        span.Pointer[i] = i;
    }
    
    // Act
    auto result = SpanSlice(span, sliceOffset, sliceLength);

    // Assert
    TestAssertEquals(sliceLength, result.Length);

    for (uint32_t i = 0; i < result.Length; i++)
    {
        TestAssertEquals(span.Pointer[i + sliceOffset], result.Pointer[i]);
    }
}

Test(Memory, SpanSliceFrom_WithSpan_HasCorrectValues)
{
    // Arrange
    const uint32_t itemCount = 10;
    const uint32_t sliceOffset = 2;

    auto span = StackAllocUint32(itemCount);

    for (uint32_t i = 0; i < itemCount; i++)
    {
        span.Pointer[i] = i;
    }
    
    // Act
    auto result = SpanSliceFrom(span, sliceOffset);

    // Assert
    TestAssertEquals(itemCount - sliceOffset, result.Length);

    for (uint32_t i = 0; i < result.Length; i++)
    {
        TestAssertEquals(span.Pointer[i + sliceOffset], result.Pointer[i]);
    }
}

Test(Memory, MemorySet_WithUint32_HasCorrectValues)
{
    // Arrange
    const uint32_t itemCount = 10;
    const uint32_t initialValue = 28;

    auto destination = StackAllocUint32(itemCount);
    
    // Act
    MemorySet(destination, initialValue);

    // Assert
    for (uint32_t i = 0; i < itemCount; i++)
    {
        TestAssertEquals(initialValue, destination.Pointer[i]);
    }
}

Test(Memory, MemorySet_WithUint8_HasCorrectValues)
{
    // Arrange
    const uint8_t itemCount = 10;
    const uint8_t initialValue = 28;

    auto destination = StackAllocUint8(itemCount);

    // Act
    MemorySet(destination, initialValue);

    // Assert
    for (uint32_t i = 0; i < itemCount; i++)
    {
        TestAssertEquals(initialValue, destination.Pointer[i]);
    }
}

Test(Memory, MemoryCopy_WithUint32_HasCorrectValues)
{
    // Arrange
    const uint8_t itemCount = 10;
    auto source = StackAllocUint32(itemCount);

    for (uint32_t i = 0; i < itemCount; i++)
    {
        source.Pointer[i] = i;
    }
    
    auto destination = StackAllocUint32(itemCount);
    MemorySet(destination, 0);
    
    // Act
    MemoryCopy(destination, ToReadOnlySpan(source));

    // Assert
    for (uint32_t i = 0; i < itemCount; i++)
    {
        TestAssertEquals(i, destination.Pointer[i]);
    }
}

Test(Memory, MemoryCopy_WithUint8_HasCorrectValues)
{
    // Arrange
    const uint8_t itemCount = 10;
    auto source = StackAllocUint8(itemCount);

    for (uint32_t i = 0; i < itemCount; i++)
    {
        source.Pointer[i] = i;
    }
    
    auto destination = StackAllocUint8(itemCount);
    MemorySet(destination, 0);
    
    // Act
    MemoryCopy(destination, ToReadOnlySpan(source));

    // Assert
    for (uint32_t i = 0; i < itemCount; i++)
    {
        TestAssertEquals(i, destination.Pointer[i]);
    }
}
