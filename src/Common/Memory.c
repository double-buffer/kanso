#include "Memory.h"
#include "Console.h"
#include "System.h"
#include "Types.h"
#include "String.h"

// TODO: This will need to be thread local
// TODO: Implement memory arena multi-threading

MemoryError globalMemoryError = MemoryError_None;

//---------------------------------------------------------------------------------------
// MemoryArena
//---------------------------------------------------------------------------------------

#define STACK_MEMORY_ARENA_DEFAULT_SIZE MegaBytesToBytes(4)

typedef struct MemoryArenaStorage
{
    MemoryReservation MemoryReservation;
    BitArray PageCommittedStatus;
    SpanUint8 DataSpan;
    uint8_t* CurrentPointer;
    size_t CommittedBytes;
    uint8_t StackLevel;
    uint8_t StackMinAllocatedLevel;
} MemoryArenaStorage;

// TODO: This will need to be thread local
MemoryArenaStorage* globalStackMemoryArenaStorage = nullptr;
MemoryArenaStorage* globalStackMemoryArenaExtraStorage = nullptr;

MemoryArenaStorage* CreateMemoryArenaStorage(size_t sizeInBytes)
{
    if (sizeInBytes == 0)
    {
        globalMemoryError = MemoryError_InvalidParameter;
        return nullptr;
    }

    auto systemInformation = SystemGetInformation();

    auto dataPageCount = DivRoundUp(sizeInBytes, systemInformation.PageSize);
    auto committedStatusBitArraySizeInBytes = AlignUp(DivRoundUp(dataPageCount, 8), alignof(size_t));

    auto storageSize = AlignUp(sizeof(MemoryArenaStorage), alignof(size_t));
    auto headerSizeInBytes = storageSize + committedStatusBitArraySizeInBytes;
    auto headerPageCount = DivRoundUp(headerSizeInBytes, systemInformation.PageSize);

    auto memoryReservation = MemoryReservePages(headerPageCount + dataPageCount);

    if (MemoryReservationIsEmpty(memoryReservation))
    {
        return nullptr;
    }

    MemoryCommitPages(&memoryReservation, 0, headerPageCount, MemoryAccess_ReadWrite);

    auto storage = (MemoryArenaStorage*)memoryReservation.BaseAddress;
    *storage = (MemoryArenaStorage){};
    storage->MemoryReservation = memoryReservation;
    storage->DataSpan = CreateSpan(uint8_t, (uint8_t*)memoryReservation.BaseAddress + headerSizeInBytes, sizeInBytes);
    
    auto committedStatusBitArrayData = SpanCast(size_t, CreateSpan(uint8_t, (uint8_t*)memoryReservation.BaseAddress + storageSize, committedStatusBitArraySizeInBytes));
    storage->PageCommittedStatus = CreateBitArrayWithBitCount(committedStatusBitArrayData, dataPageCount);
    storage->CurrentPointer = storage->DataSpan.Pointer;
    storage->StackLevel = 0;
    storage->StackMinAllocatedLevel = UINT8_MAX;

    globalMemoryError = MemoryError_None;

    return storage;
}

MemoryArenaStorage* GetMemoryArenaWorkingStorage(MemoryArena memoryArena)
{
    auto storage = memoryArena.Storage;
    
    if (memoryArena.StackLevel != storage->StackLevel)
    {
        if (!globalStackMemoryArenaExtraStorage)
        {
            // TODO: Replace the size by a constant depending on the boot phase
            // TODO: Do an util method for bytes
            globalStackMemoryArenaExtraStorage = CreateMemoryArenaStorage(STACK_MEMORY_ARENA_DEFAULT_SIZE);
        }

        // TODO: Replace by a min function
        storage->StackMinAllocatedLevel = memoryArena.StackLevel < storage->StackMinAllocatedLevel ? memoryArena.StackLevel : storage->StackMinAllocatedLevel;
        return globalStackMemoryArenaExtraStorage;
    }

    return storage;
}

MemoryArena CreateMemoryArena(size_t sizeInBytes)
{
    auto storage = CreateMemoryArenaStorage(sizeInBytes);

    if (!storage)
    {
        return MEMORY_ARENA_EMPTY;
    }

    return (MemoryArena)
    {
        .Storage = storage
    };
}

bool MemoryArenaRelease(MemoryArena* memoryArena)
{
    if (MemoryArenaIsEmpty(*memoryArena))
    {
        globalMemoryError = MemoryError_InvalidParameter;
        return false;
    }
    
    MemoryRelease(&memoryArena->Storage->MemoryReservation);

    *memoryArena = MEMORY_ARENA_EMPTY;
    globalMemoryError = MemoryError_None;

    return true;
}

MemoryArenaAllocationInfos MemoryArenaGetAllocationInfos(MemoryArena memoryArena)
{
    auto storage = memoryArena.Storage;

    return (MemoryArenaAllocationInfos)
    {
        .AllocatedBytes = storage->CurrentPointer - storage->DataSpan.Pointer,
        .CommittedBytes = storage->CommittedBytes,
        .MaximumSizeInBytes = storage->DataSpan.Length
    };
}

SpanUint8 MemoryArenaPush(MemoryArena memoryArena, size_t sizeInBytes)
{
    auto span = MemoryArenaPushReserved(memoryArena, sizeInBytes);

    if (span.Pointer)
    {
        MemoryArenaCommit(memoryArena, span);
    }

    return span;
}

SpanUint8 MemoryArenaPushReserved(MemoryArena memoryArena, size_t sizeInBytes)
{
    sizeInBytes = AlignUp(sizeInBytes, sizeof(uintptr_t));

    auto storage = GetMemoryArenaWorkingStorage(memoryArena);
    auto allocationInfos = MemoryArenaGetAllocationInfos((MemoryArena){ .Storage = storage });

    if (allocationInfos.AllocatedBytes + sizeInBytes > allocationInfos.MaximumSizeInBytes)
    {
        globalMemoryError = MemoryError_OutOfMemory;
        return CreateSpan(uint8_t, nullptr, 0);
    }

    auto span = CreateSpan(uint8_t, storage->CurrentPointer, sizeInBytes);

    storage->CurrentPointer += sizeInBytes;
    globalMemoryError = MemoryError_None;

    return span;
}

bool MemoryArenaPop(MemoryArena memoryArena, size_t sizeInBytes)
{
    if ((size_t)(memoryArena.Storage->CurrentPointer - memoryArena.Storage->DataSpan.Pointer) < sizeInBytes)
    {
        globalMemoryError = MemoryError_InvalidParameter;
        return false;
    }

    memoryArena.Storage->CurrentPointer -= sizeInBytes;
    globalMemoryError = MemoryError_None;

    return true;
}

void MemoryArenaClear(MemoryArena memoryArena)
{
    // TODO: Do nothing for stack

    memoryArena.Storage->CurrentPointer = memoryArena.Storage->DataSpan.Pointer;
    globalMemoryError = MemoryError_None;
}

bool MemoryArenaCommit(MemoryArena memoryArena, SpanUint8 range)
{
    auto storage = GetMemoryArenaWorkingStorage(memoryArena);
    auto systemInformation = SystemGetInformation();
    
    if (range.Pointer > storage->CurrentPointer || (range.Pointer + range.Length) > storage->CurrentPointer)
    {
        globalMemoryError = MemoryError_InvalidParameter;
        return false;
    }

    auto headerPageCount = ((uintptr_t)storage->DataSpan.Pointer - (uintptr_t)storage->MemoryReservation.BaseAddress) / systemInformation.PageSize;
    auto dataPageOffset = ((uintptr_t)range.Pointer - (uintptr_t)storage->DataSpan.Pointer) / systemInformation.PageSize;

    auto pageOffset = headerPageCount + dataPageOffset;
    auto pageCount = DivRoundUp(range.Length, systemInformation.PageSize);

    for (uint32_t i = 0; i < pageCount; i++)
    {
        // TODO: optimize
        if (!BitArrayIsSet(storage->PageCommittedStatus, dataPageOffset + i))
        {
            MemoryCommitPages(&storage->MemoryReservation, pageOffset + i, 1, MemoryAccess_ReadWrite);
            BitArraySet(storage->PageCommittedStatus, dataPageOffset + i);
            storage->CommittedBytes += systemInformation.PageSize;
        }
    }

    globalMemoryError = MemoryError_None;
    return true;
}

MemoryArena GetStackMemoryArena()
{
    if (!globalStackMemoryArenaStorage)
    {
        // TODO: Replace the size by a constant depending on the boot phase
        // TODO: Do an util method for bytes
        globalStackMemoryArenaStorage = CreateMemoryArenaStorage(STACK_MEMORY_ARENA_DEFAULT_SIZE);
    }

    globalStackMemoryArenaStorage->StackLevel++;

    return (MemoryArena)
    {
        .Storage = globalStackMemoryArenaStorage,
        .StackStartPointer = globalStackMemoryArenaStorage->CurrentPointer,
        .StackExtraStartPointer = globalStackMemoryArenaExtraStorage ? globalStackMemoryArenaExtraStorage->CurrentPointer : nullptr,
        .StackLevel = globalStackMemoryArenaStorage->StackLevel
    };
}

void ReleaseStackMemoryArena(MemoryArena* stackMemoryArena)
{
    if (!stackMemoryArena || !stackMemoryArena->Storage) 
    {
        return;
    }

    auto storage = stackMemoryArena->Storage;

    if (globalStackMemoryArenaExtraStorage && storage->StackMinAllocatedLevel >= stackMemoryArena->StackLevel)
    {
        size_t extraBytesToPop = (size_t)(globalStackMemoryArenaExtraStorage->CurrentPointer - stackMemoryArena->StackExtraStartPointer);

        if (extraBytesToPop)
        {
            // TODO: Do something cleaner?
            MemoryArenaPop((MemoryArena){ .Storage = globalStackMemoryArenaExtraStorage }, extraBytesToPop);
        }

        storage->StackMinAllocatedLevel = UINT8_MAX;
    }

    storage->StackLevel--;

    auto bytesToPop = (size_t)(storage->CurrentPointer - stackMemoryArena->StackStartPointer);

    if (bytesToPop) 
    {
        MemoryArenaPop(*stackMemoryArena, bytesToPop);
    }

    stackMemoryArena->StackStartPointer = storage->CurrentPointer;
    stackMemoryArena->StackExtraStartPointer = globalStackMemoryArenaExtraStorage ? globalStackMemoryArenaExtraStorage->CurrentPointer : nullptr;
}

//---------------------------------------------------------------------------------------
// General
//---------------------------------------------------------------------------------------

void MemorySetByte(size_t stride, void* destination, size_t destinationLength, const void* value)
{
    (void)stride;
    uint8_t byteValue = *(uint8_t*)value;
    __builtin_memset(destination, byteValue, destinationLength);   
}

void MemorySetDefault(size_t stride, void* destination, size_t destinationLength, const void* value)
{
    uint8_t* pointer = destination;

    for (size_t i = 0; i < destinationLength; i++)
    {
        for (size_t j = 0; j < stride; j++)
        {
            pointer[(i * stride) + j] = ((uint8_t*)value)[j];
        }
    }
}

void MemoryCopyByte(size_t stride, void* destination, size_t destinationLength, const void* source, size_t sourceLength)
{
    (void)stride;

    // TODO: Check length
    (void)destinationLength;

    __builtin_memcpy(destination, source, sourceLength);
}

void MemoryCopyDefault(size_t stride, void* destination, size_t destinationLength, const void* source, size_t sourceLength)
{
    uint8_t* pointer = destination;

    // TODO: Check length
    (void)destinationLength;

    for (size_t i = 0; i < sourceLength; i++)
    {
        for (size_t j = 0; j < stride; j++)
        {
            pointer[(i * stride) + j] = ((uint8_t*)source)[(i * stride) + j];
        }
    }
}

void* MemoryConcatChar(MemoryArena memoryArena, size_t stride, const void* source1, size_t source1Length, const void* source2, size_t source2Length)
{
    auto destination = MemoryArenaPush(memoryArena, source1Length + source2Length + 1);

    MemoryCopyByte(stride, destination.Pointer, source1Length, source1, source1Length);
    MemoryCopyByte(stride, destination.Pointer + (source1Length * stride), source2Length, source2, source2Length);

    SpanAt(destination, source1Length + source2Length) = 0;

    return destination.Pointer;
}

void* MemoryConcatByte(MemoryArena memoryArena, size_t stride, const void* source1, size_t source1Length, const void* source2, size_t source2Length)
{
    auto destination = MemoryArenaPush(memoryArena, source1Length + source2Length);

    MemoryCopyByte(stride, destination.Pointer, source1Length, source1, source1Length);
    MemoryCopyByte(stride, destination.Pointer + source1Length, source2Length, source2, source2Length);

    return destination.Pointer;
}

void* MemoryConcatDefault(MemoryArena memoryArena, size_t stride, const void* source1, size_t source1Length, const void* source2, size_t source2Length)
{
    auto destination = MemoryArenaPush(memoryArena, stride * (source1Length + source2Length));

    MemoryCopyDefault(stride, destination.Pointer, source1Length, source1, source1Length);
    MemoryCopyDefault(stride, destination.Pointer + (source1Length * stride), source2Length, source2, source2Length);

    return destination.Pointer;
}


// TODO: Move that to the standard library

size_t strlen(const char* string)
{
    const char* pointer = string;

    while (*pointer)
    {
        ++pointer;
    }

    return (size_t)(pointer - string);
}

void* memset(uint8_t* destination, uint8_t value, size_t sizeInBytes) 
{
    for (size_t i = 0; i < sizeInBytes; i++)
    {
        destination[i] = value;
    }

    return destination;
}

void* memcpy(uint8_t* destination, const uint8_t* source, size_t sizeInBytes)
{
    for (size_t i = 0; i < sizeInBytes; i++)
    {
        destination[i] = source[i];
    }

    return destination;
}
