#include "Test.h"
#include "Memory.h"

Test(Memory, MemorySet_WithUint32_HasCorrectValues)
{
    // Arrange
    uint32_t test[10];
    auto testSpan = Span(test, 10);
    
    // Act
    MemorySet(testSpan, 28);

    // Assert
    for (uint32_t i = 0; i < 10; i++)
    {
        TestAssert(false);
        // TODO: Assert
        //KernelConsolePrint("%d: %d, ", i, test[i]);
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
        // TODO: Assert
        //KernelConsolePrint("%d: %d, ", i, test[i]);
    }
}
