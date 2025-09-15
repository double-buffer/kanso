#include "Memory.h"
#include "Test.h"
#include "Types.h"

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

Test(Memory, MemoryArenaPushStruct_WithValidStruct_ReturnsValidStruct)
{
    // Arrange
    const size_t memoryArenaSize = 1024;
    auto memoryArena = CreateMemoryArena(memoryArenaSize);
    auto beforeAllocationInfos = MemoryArenaGetAllocationInfos(memoryArena);

    // Act
    auto result = MemoryArenaPushStruct(uint32_t, memoryArena);

    // Assert
    TestAssertEquals(MemoryError_None, MemoryGetLastError());
    TestAssertNotEquals(nullptr, result); 

    auto afterAllocationInfos = MemoryArenaGetAllocationInfos(memoryArena);
    TestAssertGreaterThan(afterAllocationInfos.CommittedBytes, beforeAllocationInfos.CommittedBytes);
    TestAssertEquals(sizeof(uint32_t), afterAllocationInfos.AllocatedBytes);
}

Test(Memory, MemoryArenaPushArray_WithValidStructAndCount_ReturnsValidArray)
{
    // Arrange
    const size_t memoryArenaSize = 1024;
    const uint32_t arrayCount = 5;
    auto memoryArena = CreateMemoryArena(memoryArenaSize);
    auto beforeAllocationInfos = MemoryArenaGetAllocationInfos(memoryArena);

    // Act
    auto result = MemoryArenaPushArray(uint32_t, memoryArena, arrayCount);

    // Assert
    TestAssertEquals(MemoryError_None, MemoryGetLastError());
    TestAssertNotEquals(nullptr, result.Pointer); 
    TestAssertEquals(arrayCount, result.Length); 

    auto afterAllocationInfos = MemoryArenaGetAllocationInfos(memoryArena);
    TestAssertGreaterThan(afterAllocationInfos.CommittedBytes, beforeAllocationInfos.CommittedBytes);
    TestAssertEquals(sizeof(uint32_t) * arrayCount, afterAllocationInfos.AllocatedBytes);
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

Test(Memory, MemoryArenaPop_WithValidSize_ReturnsTrue)
{
    // Arrange
    const size_t memoryArenaSize = 1024;
    const size_t pushSize = 512;
    auto memoryArena = CreateMemoryArena(memoryArenaSize);
    MemoryArenaPush(memoryArena, pushSize);

    // Act
    auto result = MemoryArenaPop(memoryArena, pushSize / 2);

    // Assert
    TestAssertEquals(MemoryError_None, MemoryGetLastError());
    TestAssertIsTrue(result);

    auto allocationInfos = MemoryArenaGetAllocationInfos(memoryArena);
    TestAssertEquals(pushSize / 2, allocationInfos.AllocatedBytes);
}

Test(Memory, MemoryArenaPop_WithInvalidSize_ReturnsFalse)
{
    // Arrange
    const size_t memoryArenaSize = 1024;
    const size_t pushSize = 512;
    auto memoryArena = CreateMemoryArena(memoryArenaSize);
    MemoryArenaPush(memoryArena, pushSize);

    // Act
    auto result = MemoryArenaPop(memoryArena, pushSize * 2);

    // Assert
    TestAssertEquals(MemoryError_InvalidParameter, MemoryGetLastError());
    TestAssertIsFalse(result);

    auto allocationInfos = MemoryArenaGetAllocationInfos(memoryArena);
    TestAssertEquals(pushSize, allocationInfos.AllocatedBytes);
}

Test(Memory, MemoryArenaClear_ResetAllocatedMemory)
{
    // Arrange
    const size_t memoryArenaSize = 1024;
    const size_t pushSize = 512;
    auto memoryArena = CreateMemoryArena(memoryArenaSize);
    MemoryArenaPush(memoryArena, pushSize);

    // Act
    MemoryArenaClear(memoryArena);

    // Assert
    TestAssertEquals(MemoryError_None, MemoryGetLastError());

    auto allocationInfos = MemoryArenaGetAllocationInfos(memoryArena);
    TestAssertEquals(0, allocationInfos.AllocatedBytes);
}

Test(Memory, MemoryArenaCommit_WithValidRange_CommitMemory)
{
    // Arrange
    const size_t memoryArenaSize = 1024;
    const size_t pushSize = 512;
    auto memoryArena = CreateMemoryArena(memoryArenaSize);
    auto span = MemoryArenaPushReserved(memoryArena, pushSize);
    auto beforeAllocationInfos = MemoryArenaGetAllocationInfos(memoryArena);

    // Act
    auto result = MemoryArenaCommit(memoryArena, SpanSlice(span, 64, 128));

    // Assert
    TestAssertEquals(MemoryError_None, MemoryGetLastError());
    TestAssertIsTrue(result); 

    auto afterAllocationInfos = MemoryArenaGetAllocationInfos(memoryArena);
    TestAssertGreaterThan(afterAllocationInfos.CommittedBytes, beforeAllocationInfos.CommittedBytes);
}

Test(Memory, MemoryArenaCommit_WithInvalidRange_CommitMemory)
{
    // Arrange
    const size_t memoryArenaSize = 1024;
    const size_t pushSize = 512;
    auto memoryArena = CreateMemoryArena(memoryArenaSize);
    auto span = MemoryArenaPushReserved(memoryArena, pushSize);
    auto beforeAllocationInfos = MemoryArenaGetAllocationInfos(memoryArena);

    // Act
    auto result = MemoryArenaCommit(memoryArena, SpanSlice(span, 512, 128));

    // Assert
    TestAssertEquals(MemoryError_InvalidParameter, MemoryGetLastError());
    TestAssertIsFalse(result); 

    auto afterAllocationInfos = MemoryArenaGetAllocationInfos(memoryArena);
    TestAssertEquals(afterAllocationInfos.CommittedBytes, beforeAllocationInfos.CommittedBytes);
}
