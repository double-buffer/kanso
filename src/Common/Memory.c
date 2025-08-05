#include "Memory.h"
#include "System.h"
#include "Types.h"

// TODO: This will need to be thread local
// TODO: Implement memory arena multi-threading

MemoryError globalMemoryError = MemoryError_None;

//---------------------------------------------------------------------------------------
// Span
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

//---------------------------------------------------------------------------------------
// MemoryArena
//---------------------------------------------------------------------------------------

typedef struct MemoryArenaStorage
{
    MemoryReservation MemoryReservation;
    BitArray PageCommittedStatus;
    SpanUint8 DataSpan;
    uint8_t* CurrentPointer;
    size_t CommittedBytes;
} MemoryArenaStorage;

MemoryArena CreateMemoryArena(size_t sizeInBytes)
{
    if (sizeInBytes == 0)
    {
        globalMemoryError = MemoryError_InvalidParameter;
        return MEMORY_ARENA_EMPTY;
    }

    auto systemInformation = SystemGetInformation();

    auto dataPageCount = DivRoundUp(sizeInBytes, systemInformation.PageSize);
    auto committedStatusBitArraySizeInBytes = DivRoundUp(dataPageCount, 8);

    auto headerSizeInBytes = sizeof(MemoryArenaStorage) + committedStatusBitArraySizeInBytes;
    auto headerPageCount = DivRoundUp(headerSizeInBytes, systemInformation.PageSize);

    auto memoryReservation = MemoryReservePages(headerPageCount + dataPageCount);

    if (MemoryReservationIsEmpty(memoryReservation))
    {
        return MEMORY_ARENA_EMPTY;
    }

    MemoryCommitPages(&memoryReservation, 0, headerPageCount, MemoryAccess_ReadWrite);

    auto storage = (MemoryArenaStorage*)memoryReservation.BaseAddress;
    *storage = (MemoryArenaStorage){};
    storage->MemoryReservation = memoryReservation;
    storage->DataSpan = CreateSpanUint8((uint8_t*)memoryReservation.BaseAddress + headerSizeInBytes, sizeInBytes);
    
    auto committedStatusBitArrayData = SpanCastSize(CreateSpanUint8((uint8_t*)memoryReservation.BaseAddress + sizeof(MemoryArenaStorage), committedStatusBitArraySizeInBytes));
    storage->PageCommittedStatus = CreateBitArrayWithBitCount(committedStatusBitArrayData, dataPageCount);
    storage->CurrentPointer = storage->DataSpan.Pointer;

    globalMemoryError = MemoryError_None;

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
    MemoryArenaCommit(memoryArena, span);

    return span;
}

SpanUint8 MemoryArenaPushReserved(MemoryArena memoryArena, size_t sizeInBytes)
{
    auto allocationInfos = MemoryArenaGetAllocationInfos(memoryArena);

    if (allocationInfos.AllocatedBytes + sizeInBytes > allocationInfos.MaximumSizeInBytes)
    {
        globalMemoryError = MemoryError_OutOfMemory;
        return CreateSpanUint8(nullptr, 0);
    }

    auto span = CreateSpanUint8(memoryArena.Storage->CurrentPointer, sizeInBytes);

    memoryArena.Storage->CurrentPointer += sizeInBytes;
    globalMemoryError = MemoryError_None;

    return span;
}

bool MemoryArenaCommit(MemoryArena memoryArena, SpanUint8 range)
{
    auto storage = memoryArena.Storage;
    auto systemInformation = SystemGetInformation();
    
    // TODO: check calculations

    auto pageOffset = (range.Pointer - storage->DataSpan.Pointer) / systemInformation.PageSize;
    auto pageCount = DivRoundUp(range.Length, systemInformation.PageSize);

    for (uint32_t i = 0; i < pageCount; i++)
    {
        // TODO: optimize
        if (!BitArrayIsSet(storage->PageCommittedStatus, pageOffset + i))
        {
            MemoryCommitPages(&storage->MemoryReservation, pageOffset, 1, MemoryAccess_ReadWrite);
            BitArraySet(storage->PageCommittedStatus, pageOffset + i);
            storage->CommittedBytes += systemInformation.PageSize;
        }
    }

    return false;
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

void memset(uint8_t* destination, uint8_t value, size_t sizeInBytes) 
{
    for (size_t i = 0; i < sizeInBytes; i++)
    {
        destination[i] = value;
    }
}

void* memcpy(uint8_t* destination, const uint8_t* source, size_t sizeInBytes)
{
    for (size_t i = 0; i < sizeInBytes; i++)
    {
        destination[i] = source[i];
    }

    return destination;
}
