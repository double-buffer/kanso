#include "KernelConsole.h"
#include "Memory.h"
#include "Platform.h"

typedef struct
{
    SpanUint8 InitHeap;
    SpanUint8 Bitmap;
} KernelInitModeMemoryState;

typedef struct
{
    bool MemoryTableInitialized;
    size_t PageSize;
    KernelInitModeMemoryState InitModeMemoryState;
} KernelMemoryState;

KernelMemoryState globalKernelMemoryState = {};

static void KernelInitModeSetupBitmapAllocator(KernelInitModeMemoryState* memoryState)
{
    auto platformInformation = PlatformGetInformation();

    globalKernelMemoryState.PageSize = platformInformation.SystemInformation.PageSize;
    memoryState->InitHeap = platformInformation.InitHeap; 

    auto maxPageCount = memoryState->InitHeap.Length / globalKernelMemoryState.PageSize;
    auto bitmapStorageSizeInBytes = (maxPageCount + 7) / 8;

    memoryState->Bitmap = SpanSlice(memoryState->InitHeap, 0, bitmapStorageSizeInBytes);
    auto bitmapStoragePageCount = (memoryState->Bitmap.Length + globalKernelMemoryState.PageSize - 1) / globalKernelMemoryState.PageSize;

    MemorySet(memoryState->Bitmap, 0xFF);

    for (uint32_t i = bitmapStoragePageCount; i < maxPageCount; i++)
    {
        // TODO:
    }

    KernelConsolePrint(String("HeapStart: %x Size:%d\n"), memoryState->InitHeap.Pointer, memoryState->InitHeap.Length);
    KernelConsolePrint(String("MaxPageCount: %d Bitmap Storage Page Count:%d\n"), maxPageCount, bitmapStoragePageCount);
}

MemoryReservation KernelInitModeMemoryReservePages(size_t pageCount)
{
    if (pageCount == 0)
    {
        globalMemoryError = MemoryError_InvalidParameter;
        return MEMORY_RESERVATION_EMPTY;
    }

    auto memoryState = &globalKernelMemoryState.InitModeMemoryState;

    if (memoryState->InitHeap.Pointer == nullptr)
    {
        KernelInitModeSetupBitmapAllocator(memoryState);
    }

    auto baseAddress = memoryState->InitHeap.Pointer;

    // TODO: 

    globalMemoryError = MemoryError_None;

    return (MemoryReservation)
    {
        .BaseAddress = baseAddress,
        .PageCount = pageCount
    };
}

MemoryReservation KernelMemoryReservePages(size_t pageCount)
{
    // TODO: 

    return (MemoryReservation)
    {
    };
}

MemoryReservation MemoryReservePages(size_t pageCount)
{
    if (!globalKernelMemoryState.MemoryTableInitialized)
    {
        return KernelInitModeMemoryReservePages(pageCount);
    }

    return KernelMemoryReservePages(pageCount);
}

bool MemoryRelease(MemoryReservation* memoryReservation)
{
    return false;
}

bool MemoryCommitPages(const MemoryReservation* memoryReservation, size_t pageOffset, size_t pageCount, MemoryAccess access)
{
    return false;
}

bool MemoryDecommitPages(const MemoryReservation* memoryReservation, size_t pageOffset, size_t pageCount)
{
    return false;
}
