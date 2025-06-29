#include "Memory.h"
#include "Test.h"

// TODO: For user mode only, we will wipe the committed pages to zero. So
// we need to test that

Test(Memory, MemoryReserve_WithValidSize_ReturnsMemoryReservation)
{
    // Arrange
    const size_t pageCount = 4;

    // Act
    auto memoryReservation = MemoryReservePages(pageCount);

    // Assert
    TestAssertEquals(MemoryError_None, MemoryGetLastError());

    TestAssertIsFalse(MemoryReservationIsEmpty(memoryReservation));
    TestAssertEquals(pageCount, memoryReservation.PageCount);
}

Test(Memory, MemoryReserve_WithMultipleReservations_ReturnsDifferentMemoryReservations)
{
    // Arrange
    const size_t pageCount = 4;

    // Act
    auto memoryReservation1 = MemoryReservePages(pageCount);
    auto memoryReservation2 = MemoryReservePages(pageCount);

    // Assert
    TestAssertEquals(MemoryError_None, MemoryGetLastError());

    TestAssertIsFalse(MemoryReservationIsEmpty(memoryReservation1));
    TestAssertEquals(pageCount, memoryReservation1.PageCount);

    TestAssertIsFalse(MemoryReservationIsEmpty(memoryReservation2));
    TestAssertEquals(pageCount, memoryReservation2.PageCount);

    TestAssertNotEquals(memoryReservation1.BaseAddress, memoryReservation2.BaseAddress);
}

Test(Memory, MemoryReserve_WithInvalidSize_ReturnsEmptyMemoryReservation)
{
    // Arrange
    const size_t pageCount = 0;

    // Act
    auto memoryReservation = MemoryReservePages(pageCount);

    // Assert
    TestAssertEquals(MemoryError_InvalidParameter, MemoryGetLastError());
    TestAssertIsTrue(MemoryReservationIsEmpty(memoryReservation));
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
    TestAssertEquals(MemoryError_None, MemoryGetLastError());
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
    TestAssertEquals(MemoryError_InvalidParameter, MemoryGetLastError());
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
    TestAssertEquals(MemoryError_None, MemoryGetLastError());
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
    TestAssertEquals(MemoryError_InvalidParameter, MemoryGetLastError());
    TestAssertIsFalse(result);
}

Test(Memory, MemoryRelease_WithValidMemoryReservation_ReturnsTrue)
{
    // Arrange
    const size_t pageCount = 4;
    auto memoryReservation = MemoryReservePages(pageCount);

    // Act
    auto result = MemoryRelease(&memoryReservation);

    // Assert
    TestAssertEquals(MemoryError_None, MemoryGetLastError());
    TestAssertIsTrue(result);
}

Test(Memory, MemoryRelease_WithInvalidMemoryReservation_ReturnsFalse)
{
    // Arrange
    auto memoryReservation = (MemoryReservation) {};

    // Act
    auto result = MemoryRelease(&memoryReservation);

    // Assert
    TestAssertEquals(MemoryError_InvalidParameter, MemoryGetLastError());
    TestAssertIsFalse(result);
}
