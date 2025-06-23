#include "KernelConsole.h"
#include "Memory.h"
#include "Platform.h"

SpanUint8 globalInitHeap;
bool globalMemoryTableInitialized = false;

MemoryReservation KernelInitModeMemoryReservePages(size_t pageCount)
{
    if (globalInitHeap.Pointer == nullptr)
    {
        auto platformInformation = PlatformGetInformation();
        globalInitHeap = platformInformation.InitHeap; 
        KernelConsolePrint(String("HeapStart: %x Size:%d\n"), globalInitHeap.Pointer, globalInitHeap.Length);
    }

    // TODO: 

    return (MemoryReservation)
    {
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
    if (!globalMemoryTableInitialized)
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
