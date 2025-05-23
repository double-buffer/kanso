#include "Test.h"
#include "Memory.h"

// TODO: Memory set with a span that has less elements

Test(Memory, MemorySet_WithUint32_HasCorrectValues)
{
    // Arrange
    const uint32_t itemCount = 10;
    const uint32_t initialValue = 28;

    uint32_t destination[itemCount];
    auto destinationSpan = Span(destination, itemCount);
    
    // Act
    MemorySet(destinationSpan, initialValue);

    // Assert
    for (uint32_t i = 0; i < itemCount; i++)
    {
        TestAssertEquals(initialValue, destinationSpan.Pointer[i]);
    }
}

Test(Memory, MemorySet_WithUint8_HasCorrectValues)
{
    // Arrange
    const uint8_t itemCount = 10;
    const uint8_t initialValue = 28;

    uint8_t destination[itemCount];
    auto destinationSpan = Span(destination, itemCount);

    // Act
    MemorySet(destinationSpan, initialValue);

    // Assert
    for (uint32_t i = 0; i < itemCount; i++)
    {
        TestAssertEquals(initialValue, destinationSpan.Pointer[i]);
    }
}

Test(Memory, MemoryCopy_WithUint32_HasCorrectValues)
{
    // Arrange
    const uint8_t itemCount = 10;
    uint32_t source[itemCount];

    for (uint32_t i = 0; i < itemCount; i++)
    {
        source[i] = i;
    }
    
    auto sourceSpan = ReadOnlySpan((const uint32_t*)source, itemCount);
    
    uint32_t destination[itemCount];
    auto destinationSpan = Span(destination, itemCount);
    MemorySet(destinationSpan, 0);
    
    // Act
    MemoryCopy(destinationSpan, sourceSpan);

    // Assert
    for (uint32_t i = 0; i < itemCount; i++)
    {
        TestAssertEquals(i, destination[i]);
    }
}

Test(Memory, MemoryCopy_WithUint8_HasCorrectValues)
{
    // Arrange
    const uint8_t itemCount = 10;
    uint8_t source[itemCount];

    for (uint32_t i = 0; i < itemCount; i++)
    {
        source[i] = i;
    }
    
    auto sourceSpan = ReadOnlySpan((const uint32_t*)source, itemCount);
    
    uint8_t destination[itemCount];
    auto destinationSpan = Span(destination, itemCount);
    MemorySet(destinationSpan, 0);
    
    // Act
    MemoryCopy(destinationSpan, sourceSpan);

    // Assert
    for (uint32_t i = 0; i < itemCount; i++)
    {
        TestAssertEquals(i, destination[i]);
    }
}
