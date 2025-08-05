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
    TestAssertEquals(0, allocationInfos.AllocatedBytes);
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

Test(Memory, MemoryArenaRelease_WithValidArena_ReturnsTrue)
{
    // Arrange
    const size_t memoryArenaSize = 1024;
    auto memoryArena = CreateMemoryArena(memoryArenaSize);
    auto beforeAllocationInfos = MemoryGetAllocationInfos();

    // Act
    auto result = MemoryArenaRelease(&memoryArena);

    // Assert
    TestAssertEquals(MemoryError_None, MemoryGetLastError());
    TestAssertIsTrue(result);
    TestAssertIsTrue(MemoryArenaIsEmpty(memoryArena));
    
    auto afterAllocationInfos = MemoryGetAllocationInfos();
    TestAssertGreaterThan(beforeAllocationInfos.ReservedPages, afterAllocationInfos.ReservedPages);
}

Test(Memory, MemoryArenaRelease_WithInvalidArena_ReturnsFalse)
{
    // Arrange
    auto memoryArena = (MemoryArena) {};

    // Act
    auto result = MemoryArenaRelease(&memoryArena);

    // Assert
    TestAssertEquals(MemoryError_InvalidParameter, MemoryGetLastError());
    TestAssertIsFalse(result);
}

Test(Memory, MemoryArenaPush_WithValidSize_ReturnsValidSpan)
{
    // Arrange
    const size_t memoryArenaSize = 1024;
    const size_t pushSize = 512;
    auto memoryArena = CreateMemoryArena(memoryArenaSize);
    auto beforeAllocationInfos = MemoryArenaGetAllocationInfos(memoryArena);

    // Act
    auto span = MemoryArenaPush(memoryArena, pushSize);

    // Assert
    TestAssertEquals(MemoryError_None, MemoryGetLastError());
    TestAssertNotEquals(nullptr, span.Pointer); 
    TestAssertEquals(pushSize, span.Length); 

    auto afterAllocationInfos = MemoryArenaGetAllocationInfos(memoryArena);
    TestAssertGreaterThan(afterAllocationInfos.CommittedBytes, beforeAllocationInfos.CommittedBytes);
    TestAssertEquals(pushSize, afterAllocationInfos.AllocatedBytes);
}

Test(Memory, MemoryArenaPush_WithInvalidSize_ReturnsEmptySpan)
{
    // Arrange
    const size_t memoryArenaSize = 1024;
    const size_t pushSize = 2048;
    auto memoryArena = CreateMemoryArena(memoryArenaSize);
    auto beforeAllocationInfos = MemoryArenaGetAllocationInfos(memoryArena);

    // Act
    auto span = MemoryArenaPush(memoryArena, pushSize);

    // Assert
    TestAssertEquals(MemoryError_OutOfMemory, MemoryGetLastError());
    TestAssertEquals(nullptr, span.Pointer); 
    TestAssertEquals(0, span.Length); 

    auto afterAllocationInfos = MemoryArenaGetAllocationInfos(memoryArena);
    TestAssertEquals(0, afterAllocationInfos.AllocatedBytes);
    TestAssertEquals(beforeAllocationInfos.CommittedBytes, afterAllocationInfos.CommittedBytes);
}

Test(Memory, MemoryArenaPushReserved_WithValidSize_ReturnsValidSpan)
{
    // Arrange
    const size_t memoryArenaSize = 1024;
    const size_t pushSize = 512;
    auto memoryArena = CreateMemoryArena(memoryArenaSize);
    auto beforeAllocationInfos = MemoryArenaGetAllocationInfos(memoryArena);

    // Act
    auto span = MemoryArenaPushReserved(memoryArena, pushSize);

    // Assert
    TestAssertEquals(MemoryError_None, MemoryGetLastError());
    TestAssertNotEquals(nullptr, span.Pointer); 
    TestAssertEquals(pushSize, span.Length); 

    auto afterAllocationInfos = MemoryArenaGetAllocationInfos(memoryArena);
    TestAssertEquals(afterAllocationInfos.CommittedBytes, beforeAllocationInfos.CommittedBytes);
    TestAssertEquals(pushSize, afterAllocationInfos.AllocatedBytes);
}

Test(Memory, MemoryArenaPushReserved_WithInvalidSize_ReturnsEmptySpan)
{
    // Arrange
    const size_t memoryArenaSize = 1024;
    const size_t pushSize = 2048;
    auto memoryArena = CreateMemoryArena(memoryArenaSize);
    auto beforeAllocationInfos = MemoryArenaGetAllocationInfos(memoryArena);

    // Act
    auto span = MemoryArenaPushReserved(memoryArena, pushSize);

    // Assert
    TestAssertEquals(MemoryError_OutOfMemory, MemoryGetLastError());
    TestAssertEquals(nullptr, span.Pointer); 
    TestAssertEquals(0, span.Length); 

    auto afterAllocationInfos = MemoryArenaGetAllocationInfos(memoryArena);
    TestAssertEquals(0, afterAllocationInfos.AllocatedBytes);
    TestAssertEquals(beforeAllocationInfos.CommittedBytes, afterAllocationInfos.CommittedBytes);
}

