#include "KernelConsole.h"
#include "Memory.h"
#include "Platform.h"
#include "Types.h"

typedef struct
{
    SpanUint8 InitHeap;
    BitArray BitArray;
} KernelInitModeMemoryState;

typedef struct
{
    bool MemoryTableInitialized;
    KernelInitModeMemoryState InitModeMemoryState;
    uint32_t PageSize;
    size_t CommittedPages;
    size_t ReservedPages;
} KernelMemoryState;

KernelMemoryState globalKernelMemoryState = {};

static bool CheckMemoryReservationRange(const MemoryReservation* memoryReservation, size_t pageOffset, size_t pageCount)
{
    if ((pageOffset + pageCount) > memoryReservation->PageCount)
    {
        globalMemoryError = MemoryError_InvalidParameter;
        return false;
    }

    return true;
}

static void KernelInitModeSetupBitArrayAllocator()
{
    auto platformInformation = PlatformGetInformation();

    auto memoryState = &globalKernelMemoryState.InitModeMemoryState;

    globalKernelMemoryState.PageSize = platformInformation.SystemInformation.PageSize;
    memoryState->InitHeap = platformInformation.InitHeap; 

    auto maxPageCount = memoryState->InitHeap.Length / globalKernelMemoryState.PageSize;

    auto bitmapStorageSizeInBytes = DivRoundUp(maxPageCount, 8);
    auto bitmapStoragePageCount = DivRoundUp(bitmapStorageSizeInBytes, globalKernelMemoryState.PageSize);

    memoryState->BitArray = CreateBitArrayWithBitCount(SpanCastSize(SpanSlice(memoryState->InitHeap, 0, bitmapStorageSizeInBytes)), maxPageCount);

    for (uint32_t i = 0; i < maxPageCount; i++)
    {
        if (i < bitmapStoragePageCount)
        {
            BitArraySet(memoryState->BitArray, i);
        }
        else 
        {
            BitArrayReset(memoryState->BitArray, i);
        }
    }
}

MemoryReservation KernelInitModeMemoryReservePages(size_t pageCount)
{
    auto memoryState = &globalKernelMemoryState.InitModeMemoryState;

    if (memoryState->InitHeap.Pointer == nullptr)
    {
        KernelInitModeSetupBitArrayAllocator();
    }

    auto freeIndex = BitArrayFindRangeNotSet(memoryState->BitArray, pageCount);

    for (uint32_t i = 0; i < pageCount; i++)
    {
        BitArraySet(memoryState->BitArray, freeIndex + i);
    }

    auto baseAddress = memoryState->InitHeap.Pointer + (freeIndex * globalKernelMemoryState.PageSize);
    globalMemoryError = MemoryError_None;

    globalKernelMemoryState.ReservedPages += pageCount;

    return (MemoryReservation)
    {
        .BaseAddress = baseAddress,
        .PageCount = pageCount
    };
}

bool KernelInitModeMemoryReleasePages(MemoryReservation* memoryReservation)
{
    auto memoryState = &globalKernelMemoryState.InitModeMemoryState;

    if (memoryState->InitHeap.Pointer == nullptr)
    {
        globalMemoryError = MemoryError_InvalidParameter;
        return false;
    }

    auto startIndex = ((uint8_t*)memoryReservation->BaseAddress - memoryState->InitHeap.Pointer) / globalKernelMemoryState.PageSize;

    for (uint32_t i = 0; i < memoryReservation->PageCount; i++)
    {
        BitArrayReset(memoryState->BitArray, startIndex + i);
    }

    globalKernelMemoryState.ReservedPages -= memoryReservation->PageCount;

    globalMemoryError = MemoryError_None;
    *memoryReservation = (MemoryReservation){};

    return true;
}

MemoryReservation KernelMemoryReservePages(size_t pageCount)
{
    // TODO: 

    return (MemoryReservation)
    {
    };
}

MemoryAllocationInfos MemoryGetAllocationInfos()
{
    return (MemoryAllocationInfos)
    {
        .CommittedPages = globalKernelMemoryState.CommittedPages,
        .ReservedPages = globalKernelMemoryState.ReservedPages
    };
}

MemoryReservation MemoryReservePages(size_t pageCount)
{
    if (pageCount == 0)
    {
        globalMemoryError = MemoryError_InvalidParameter;
        return MEMORY_RESERVATION_EMPTY;
    }

    if (!globalKernelMemoryState.MemoryTableInitialized)
    {
        return KernelInitModeMemoryReservePages(pageCount);
    }

    return KernelMemoryReservePages(pageCount);
}

bool MemoryRelease(MemoryReservation* memoryReservation)
{
    if (MemoryReservationIsEmpty(*memoryReservation))
    {
        globalMemoryError = MemoryError_InvalidParameter;
        return false;
    }

    if (!globalKernelMemoryState.MemoryTableInitialized)
    {
        return KernelInitModeMemoryReleasePages(memoryReservation);
    }
    
    // TODO: Kernel Mode

    return false;
}

bool MemoryCommitPages(const MemoryReservation* memoryReservation, size_t pageOffset, size_t pageCount, MemoryAccess access)
{
    if (!CheckMemoryReservationRange(memoryReservation, pageOffset, pageCount))
    {
        return false;
    }

    if (!globalKernelMemoryState.MemoryTableInitialized)
    {
        globalMemoryError = MemoryError_None;
        return true;
    }

    // TODO: Real MMU memory commit

    return false;
}

bool MemoryDecommitPages(const MemoryReservation* memoryReservation, size_t pageOffset, size_t pageCount)
{
    if (!CheckMemoryReservationRange(memoryReservation, pageOffset, pageCount))
    {
        return false;
    }

    if (!globalKernelMemoryState.MemoryTableInitialized)
    {
        globalMemoryError = MemoryError_None;
        return true;
    }

    // TODO: Real MMU memory commit

    return false;
}
