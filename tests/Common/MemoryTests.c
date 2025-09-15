#include "Memory.h"
#include "Test.h"

// TODO: Memory set with a span that has less elements

Test(Memory, MemorySet_WithUint32_HasCorrectValues)
{
    // Arrange
    const uint32_t itemCount = 10;
    const uint32_t initialValue = 28;

    auto destination = StackAlloc(uint32_t, itemCount);
    
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

    auto destination = StackAlloc(uint8_t, itemCount);

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
    auto source = StackAlloc(uint32_t, itemCount);

    for (uint32_t i = 0; i < itemCount; i++)
    {
        SpanAt(source, i) = i;
    }
    
    auto destination = StackAlloc(uint32_t, itemCount);
    MemorySet(destination, 0);
    
    // Act
    MemoryCopy(destination, ToReadOnlySpan(uint32_t, source));

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
    auto source = StackAlloc(uint8_t, itemCount);

    for (uint32_t i = 0; i < itemCount; i++)
    {
        SpanAt(source, i) = i;
    }
    
    auto destination = StackAlloc(uint8_t, itemCount);
    MemorySet(destination, 0);
    
    // Act
    MemoryCopy(destination, ToReadOnlySpan(uint8_t, source));

    // Assert
    for (uint32_t i = 0; i < itemCount; i++)
    {
        TestAssertEquals(i, SpanAt(destination, i));
    }
}

