#include "Memory.h"
#include "Test.h"

// TODO: SpanSlice: Test Length and overflow
// TODO: Memory set with a span that has less elements

Test(Memory, SpanSlice_WithSpan_HasCorrectValues)
{
    // Arrange
    const uint32_t itemCount = 10;
    const uint32_t sliceOffset = 2;
    const uint32_t sliceLength = 5;

    auto span = StackAllocUint32(itemCount);

    for (uint32_t i = 0; i < itemCount; i++)
    {
        SpanAt(span, i) = i;
    }
    
    // Act
    auto result = SpanSlice(span, sliceOffset, sliceLength);

    // Assert
    TestAssertEquals(sliceLength, result.Length);

    for (uint32_t i = 0; i < result.Length; i++)
    {
        TestAssertEquals(SpanAt(span, i + sliceOffset), SpanAt(result, i));
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
        SpanAt(span, i) = i;
    }
    
    // Act
    auto result = SpanSliceFrom(span, sliceOffset);

    // Assert
    TestAssertEquals(itemCount - sliceOffset, result.Length);

    for (uint32_t i = 0; i < result.Length; i++)
    {
        TestAssertEquals(SpanAt(span, i + sliceOffset), SpanAt(result, i));
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

Test(Memory, MemoryReserve_WithValidSize_ReturnsMemoryReservation)
{
    // Arrange
    const size_t pageCount = 4;

    // Act
    auto memoryReservation = MemoryReservePages(pageCount);

    // Assert
    TestAssertEquals(pageCount, memoryReservation.PageCount);
    TestAssertNotEquals(nullptr, memoryReservation.BaseAddress);
}

Test(Memory, MemoryReserve_WithInvalidSize_ReturnsEmptyMemoryReservation)
{
    // Arrange
    const size_t pageCount = 0;

    // Act
    auto memoryReservation = MemoryReservePages(pageCount);

    // Assert
    TestAssertEquals(0, memoryReservation.PageCount);
    TestAssertEquals(nullptr, memoryReservation.BaseAddress);
}

Test(Memory, MemoryCommit_WithValidRange_ReturnsTrue)
{
    // Arrange
    const size_t reservedPageCount = 4;
    const size_t committedPageOffset = 1;
    const size_t committedPageCount = 2;
    const size_t memoryAccess = MemoryAccess_ReadWrite;

    auto memoryReservation = MemoryReservePages(reservedPageCount);

    // Act
    auto result = MemoryCommitPages(&memoryReservation, committedPageOffset, committedPageCount, memoryAccess);

    // Assert
    TestAssertIsTrue(result);
}

Test(Memory, MemoryCommit_WithInvalidRange_ReturnsFalse)
{
    // Arrange
    const size_t reservedPageCount = 4;
    const size_t committedPageOffset = 2;
    const size_t committedPageCount = 3;
    const size_t memoryAccess = MemoryAccess_ReadWrite;

    auto memoryReservation = MemoryReservePages(reservedPageCount);

    // Act
    auto result = MemoryCommitPages(&memoryReservation, committedPageOffset, committedPageCount, memoryAccess);

    // Assert
    TestAssertIsFalse(result);
}

Test(Memory, MemoryDecommit_WithValidRange_ReturnsTrue)
{
    // Arrange
    const size_t reservedPageCount = 4;
    const size_t committedPageOffset = 1;
    const size_t committedPageCount = 2;
    const size_t memoryAccess = MemoryAccess_ReadWrite;
    const size_t decommittedPageOffset = 1;
    const size_t decommittedPageCount = 2;

    auto memoryReservation = MemoryReservePages(reservedPageCount);
    MemoryCommitPages(&memoryReservation, committedPageOffset, committedPageCount, memoryAccess);

    // Act
    auto result = MemoryDecommitPages(&memoryReservation, decommittedPageOffset, decommittedPageCount);

    // Assert
    TestAssertIsTrue(result);
}

Test(Memory, MemoryDecommit_WithInvalidRange_ReturnsFalse)
{
    // Arrange
    const size_t reservedPageCount = 4;
    const size_t committedPageOffset = 1;
    const size_t committedPageCount = 2;
    const size_t memoryAccess = MemoryAccess_ReadWrite;
    const size_t decommittedPageOffset = 2;
    const size_t decommittedPageCount = 3;

    auto memoryReservation = MemoryReservePages(reservedPageCount);
    MemoryCommitPages(&memoryReservation, committedPageOffset, committedPageCount, memoryAccess);

    // Act
    auto result = MemoryDecommitPages(&memoryReservation, decommittedPageOffset, decommittedPageCount);

    // Assert
    TestAssertIsFalse(result);
}

Test(Memory, MemoryReserve_WithValidMemoryReservation_ReturnsTrue)
{
    // Arrange
    const size_t pageCount = 4;
    auto memoryReservation = MemoryReservePages(pageCount);

    // Act
    auto result = MemoryRelease(&memoryReservation);

    // Assert
    TestAssertIsTrue(result);
}

Test(Memory, MemoryReserve_WithInvalidMemoryReservation_ReturnsFalse)
{
    // Arrange
    const size_t pageCount = 4;
    auto memoryReservation = (MemoryReservation) {};

    // Act
    auto result = MemoryRelease(&memoryReservation);

    // Assert
    TestAssertIsTrue(result);
}
