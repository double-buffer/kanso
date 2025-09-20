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
    MemoryCopy(destination, source);

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
    MemoryCopy(destination, source);

    // Assert
    for (uint32_t i = 0; i < itemCount; i++)
    {
        TestAssertEquals(i, SpanAt(destination, i));
    }
}

Test(Memory, MemoryConcat_WithUint8_HasCorrectValues)
{
    // Arrange
    const size_t memoryArenaSize = 1024;
    const uint8_t itemCount = 10;
    const uint8_t source1Value = 5;
    const uint8_t source2Value = 28;

    auto memoryArena = CreateMemoryArena(memoryArenaSize);

    auto source1 = StackAlloc(uint8_t, itemCount);
    MemorySet(source1, source1Value);

    auto source2 = StackAlloc(uint8_t, itemCount);
    MemorySet(source2, source2Value);

    // Act
    auto result = MemoryConcat(memoryArena, ToReadOnlySpan(uint8_t, source1), source2);

    // Assert
    TestAssertEquals(MemoryError_None, MemoryGetLastError());
    TestAssertNotEquals(nullptr, result.Pointer); 
    TestAssertEquals(itemCount * 2, result.Length); 

    for (uint32_t i = 0; i < itemCount; i++)
    {
        TestAssertEquals(source1Value, SpanAt(result, i));
        TestAssertEquals(source2Value, SpanAt(result, itemCount + i));
    }
}

Test(Memory, MemoryConcat_WithChar_HasCorrectValues)
{
    // Arrange
    const size_t memoryArenaSize = 1024;
    const uint8_t itemCount = 10;
    const uint8_t defaultValue = 255;
    const uint8_t source1Value = 5;
    const uint8_t source2Value = 28;

    auto memoryArena = CreateMemoryArena(memoryArenaSize);
    auto data = MemoryArenaPush(memoryArena, memoryArenaSize);
    MemorySet(data, defaultValue);
    MemoryArenaClear(memoryArena);

    auto source1 = StackAlloc(char, itemCount);
    MemorySet(source1, source1Value);

    auto source2 = StackAlloc(char, itemCount);
    MemorySet(source2, source2Value);

    // Act
    auto result = MemoryConcat(memoryArena, source1, source2);

    // Assert
    TestAssertEquals(MemoryError_None, MemoryGetLastError());
    TestAssertNotEquals(nullptr, result.Pointer); 
    TestAssertEquals(itemCount * 2, result.Length); 

    for (uint32_t i = 0; i < itemCount; i++)
    {
        TestAssertEquals(source1Value, SpanAt(result, i));
        TestAssertEquals(source2Value, SpanAt(result, itemCount + i));
    }

    TestAssertEquals(0, SpanAt(result, result.Length));
}

Test(Memory, MemoryConcat_WithUint32_HasCorrectValues)
{
    // Arrange
    const size_t memoryArenaSize = 1024;
    const uint8_t itemCount = 10;
    const uint32_t source1Value = 5;
    const uint32_t source2Value = 28;

    auto memoryArena = CreateMemoryArena(memoryArenaSize);

    auto source1 = StackAlloc(uint32_t, itemCount);
    MemorySet(source1, source1Value);

    auto source2 = StackAlloc(uint32_t, itemCount);
    MemorySet(source2, source2Value);

    // Act
    auto result = MemoryConcat(memoryArena, source1, source2);

    // Assert
    TestAssertEquals(MemoryError_None, MemoryGetLastError());
    TestAssertNotEquals(nullptr, result.Pointer); 
    TestAssertEquals(itemCount * 2, result.Length); 

    for (uint32_t i = 0; i < itemCount; i++)
    {
        TestAssertEquals(source1Value, SpanAt(result, i));
        TestAssertEquals(source2Value, SpanAt(result, itemCount + i));
    }
}
