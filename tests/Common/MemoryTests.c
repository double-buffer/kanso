#include "Test.h"
#include "Memory.h"

Test(Memory, MemorySet_WithUint32_HasCorrectValues)
{
    // Arrange
    const uint32_t count = 10;
    uint32_t test[count];
    auto testSpan = Span(test, count);
    
    // Act
    MemorySet(testSpan, 28);

    // Assert
    for (uint32_t i = 0; i < count; i++)
    {
        TestAssertEquals(28, testSpan.Pointer[i]);
    }
}

Test(Memory, MemorySet_WithUint8_HasCorrectValues)
{
    // Arrange
    uint8_t test[10];
    auto testSpan = Span(test, 10);
    
    // Act
    MemorySet(testSpan, 28);

    // Assert
    for (uint32_t i = 0; i < 10; i++)
    {
        TestAssertEquals(28, test[i]);
    }
}

// TODO: Memory set with a span that has less elements

Test(Memory, MemoryCopy_WithUint32_HasCorrectValues)
{
    // Arrange
    uint32_t source[10];

    for (uint32_t i = 0; i < 10; i++)
    {
        source[i] = i;
    }
    
    auto test = Span(source, 10);
    auto sourceSpan = ToReadOnlySpan(test);//ReadOnlySpan((const uint32_t*)source, 10);
    
    uint32_t destination[10];
    auto destinationSpan = Span(destination, 10);
    MemorySet(destinationSpan, 0);
    
    // Act
    MemoryCopy(destinationSpan, sourceSpan);

    // Assert
    for (uint32_t i = 0; i < 10; i++)
    {
        TestAssertEquals(i, destination[i]);
    }
}

Test(Memory, MemoryCopy_WithUint8_HasCorrectValues)
{
    // Arrange
    uint8_t source[10];

    for (uint32_t i = 0; i < 10; i++)
    {
        source[i] = i;
    }
    
    auto sourceSpan = ReadOnlySpan((const uint32_t*)source, 10);
    
    uint8_t destination[10];
    auto destinationSpan = Span(destination, 10);
    MemorySet(destinationSpan, 0);
    
    // Act
    MemoryCopy(destinationSpan, sourceSpan);

    // Assert
    for (uint32_t i = 0; i < 10; i++)
    {
        TestAssertEquals(i, destination[i]);
    }
}
