#include "Memory.h"
#include "Test.h"

Test(Memory, CreateMemoryArena_WithValidSize_ReturnsMemoryArena)
{
    // Arrange
    const size_t memoryArenaSize = 1024;

    // Act
    auto memoryArena = CreateMemoryArena(memoryArenaSize);

    // Assert
    TestAssertEquals(MemoryError_None, MemoryGetLastError());
    TestAssertIsFalse(MemoryArenaIsEmpty(memoryArena));
    
    auto allocationInfos = MemoryArenaGetAllocationInfos(memoryArena);

    TestAssertEquals(memoryArenaSize, allocationInfos.MaximumSizeInBytes);
}

Test(Memory, CreateMemoryArena_WithInvalidSize_ReturnsEmptyMemoryArena)
{
    // Arrange
    const size_t memoryArenaSize = 0;

    // Act
    auto memoryArena = CreateMemoryArena(memoryArenaSize);

    // Assert
    TestAssertEquals(MemoryError_InvalidParameter, MemoryGetLastError());
    TestAssertIsTrue(MemoryArenaIsEmpty(memoryArena));
}

