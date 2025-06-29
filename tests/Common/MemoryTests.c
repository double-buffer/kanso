#include "Memory.h"
#include "Test.h"

// TODO: Memory set with a span that has less elements

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
        TestAssertEquals(initialValue, SpanAt(destination, i));
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
        TestAssertEquals(initialValue, SpanAt(destination, i));
    }
}

Test(Memory, MemoryCopy_WithUint32_HasCorrectValues)
{
    // Arrange
    const uint8_t itemCount = 10;
    auto source = StackAllocUint32(itemCount);

    for (uint32_t i = 0; i < itemCount; i++)
    {
        SpanAt(source, i) = i;
    }
    
    auto destination = StackAllocUint32(itemCount);
    MemorySet(destination, 0);
    
    // Act
    MemoryCopy(destination, ToReadOnlySpanUint32(source));

    // Assert
    for (uint32_t i = 0; i < itemCount; i++)
    {
        TestAssertEquals(i, SpanAt(destination, i));
    }
}

Test(Memory, MemoryCopy_WithUint8_HasCorrectValues)
{
    // Arrange
    const uint8_t itemCount = 10;
    auto source = StackAllocUint8(itemCount);

    for (uint32_t i = 0; i < itemCount; i++)
    {
        SpanAt(source, i) = i;
    }
    
    auto destination = StackAllocUint8(itemCount);
    MemorySet(destination, 0);
    
    // Act
    MemoryCopy(destination, ToReadOnlySpanUint8(source));

    // Assert
    for (uint32_t i = 0; i < itemCount; i++)
    {
        TestAssertEquals(i, SpanAt(destination, i));
    }
}

