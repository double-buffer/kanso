#pragma once

#include "Types.h"

//---------------------------------------------------------------------------------------
// Error Handling
//---------------------------------------------------------------------------------------

typedef enum 
{
    MemoryError_None,
    MemoryError_InvalidParameter,
    MemoryError_OutOfMemory
} MemoryError;

// TODO: This will need to be thread local
extern MemoryError globalMemoryError;

static inline MemoryError MemoryGetLastError()
{
    return globalMemoryError;
}

//---------------------------------------------------------------------------------------
// Utilities
//---------------------------------------------------------------------------------------

#define KiloBytesToBytes(value) (value) * 1024
#define MegaBytesToBytes(value) KiloBytesToBytes((value)) * 1024
#define GigaBytesToBytes(value) MegaBytesToBytes((value)) * 1024

//---------------------------------------------------------------------------------------
// Memory Allocation
//---------------------------------------------------------------------------------------

typedef enum
{
    MemoryAccess_Read,
    MemoryAccess_ReadWrite,
    MemoryAccess_Execute,
    MemoryAccess_ExecuteRead,
    MemoryAccess_ExecuteReadWrite
} MemoryAccess;

typedef struct
{
    void* BaseAddress;
    size_t PageCount;
} MemoryReservation;

typedef struct 
{
    size_t CommittedPages;
    size_t ReservedPages;
} MemoryAllocationInfos;

#define MEMORY_RESERVATION_EMPTY ((MemoryReservation) { .BaseAddress = nullptr, .PageCount = 0 })

static inline bool MemoryReservationIsEmpty(MemoryReservation memoryReservation)
{
    return memoryReservation.BaseAddress == nullptr;
}

MemoryAllocationInfos MemoryGetAllocationInfos();

MemoryReservation MemoryReservePages(size_t pageCount);
bool MemoryRelease(MemoryReservation* memoryReservation);

bool MemoryCommitPages(const MemoryReservation* memoryReservation, size_t pageOffset, size_t pageCount, MemoryAccess access);
bool MemoryDecommitPages(const MemoryReservation* memoryReservation, size_t pageOffset, size_t pageCount);

//---------------------------------------------------------------------------------------
// Memory Arena
//---------------------------------------------------------------------------------------

struct MemoryArenaStorage;

typedef struct
{
    struct MemoryArenaStorage* Storage;
    uint8_t* StackStartPointer;
    uint8_t* StackExtraStartPointer;
    uint8_t StackLevel;
} MemoryArena;

typedef struct 
{
    size_t AllocatedBytes;
    size_t CommittedBytes;
    size_t MaximumSizeInBytes;
} MemoryArenaAllocationInfos;

#define MEMORY_ARENA_EMPTY ((MemoryArena) { .Storage = nullptr });

static inline bool MemoryArenaIsEmpty(MemoryArena memoryArena)
{
    return memoryArena.Storage == nullptr;
}

MemoryArena CreateMemoryArena(size_t sizeInBytes);
bool MemoryArenaRelease(MemoryArena* memoryArena);

MemoryArenaAllocationInfos MemoryArenaGetAllocationInfos(MemoryArena memoryArena);

SpanUint8 MemoryArenaPush(MemoryArena memoryArena, size_t sizeInBytes);
SpanUint8 MemoryArenaPushReserved(MemoryArena memoryArena, size_t sizeInBytes);

#define MemoryArenaPushStruct(type, memoryArena) ((type*)MemoryArenaPush((memoryArena), sizeof(type)).Pointer)
#define MemoryArenaPushArray(type, memoryArena, count) CreateSpan(type, ((type*)MemoryArenaPush((memoryArena), sizeof(type) * (count)).Pointer), (count))

bool MemoryArenaPop(MemoryArena memoryArena, size_t sizeInBytes);
void MemoryArenaClear(MemoryArena memoryArena);

bool MemoryArenaCommit(MemoryArena memoryArena, SpanUint8 range);

MemoryArena GetStackMemoryArena();
void ReleaseStackMemoryArena(MemoryArena* stackMemoryArena);

#define StackMemoryArena(name) [[gnu::cleanup(ReleaseStackMemoryArena)]] MemoryArena name = GetStackMemoryArena();

//---------------------------------------------------------------------------------------
// General
//---------------------------------------------------------------------------------------

void MemorySetByte(size_t stride, void* destination, size_t destinationLength, const void* value);
void MemorySetDefault(size_t stride, void* destination, size_t destinationLength, const void* value);

#define MemorySet(destination, value) \
    _Generic((destination).Pointer, \
        char*: MemorySetByte, \
        uint8_t*: MemorySetByte, \
        default: MemorySetDefault \
    )(sizeof(*(destination).Pointer), (destination).Pointer, (destination).Length, &(typeof(*(destination).Pointer)){ (value) })


// TODO: Add Errors

void MemoryCopyByte(size_t stride, void* destination, size_t destinationLength, const void* source, size_t sourceLength);
void MemoryCopyDefault(size_t stride, void* destination, size_t destinationLength, const void* source, size_t sourceLength);

#define MemoryCopy(destination, source) \
    _Generic((destination).Pointer, \
        char*: MemoryCopyByte, \
        uint8_t*: MemoryCopyByte, \
        default: MemoryCopyDefault \
    )(sizeof(*(destination).Pointer), (destination).Pointer, (destination).Length, (source).Pointer, (source).Length)

void* MemoryConcatChar(MemoryArena memoryArena, size_t stride, const void* source1, size_t source1Length, const void* source2, size_t source2Length);
void* MemoryConcatByte(MemoryArena memoryArena, size_t stride, const void* source1, size_t source1Length, const void* source2, size_t source2Length);
void* MemoryConcatDefault(MemoryArena memoryArena, size_t stride, const void* source1, size_t source1Length, const void* source2, size_t source2Length);

// TODO: It works but the only drawback now is that if source1 is a ReadOnlySpan, it will create a ReadOnlySpan as a result
#define MemoryConcat(memoryArena, source1, source2) \
        (typeof(source1)) \
        { \
            .Pointer = _Generic((source1).Pointer, \
                    char*: MemoryConcatChar, \
                    const char*: MemoryConcatChar, \
                    uint8_t*: MemoryConcatByte, \
                    const uint8_t*: MemoryConcatByte, \
                    default: MemoryConcatDefault \
                )(memoryArena, sizeof(*(source1).Pointer), (source1).Pointer, (source1).Length, (source2).Pointer, (source2).Length), \
            .Length  = (source1).Length + (source2).Length \
        };
