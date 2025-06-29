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

#define _MemoryCopyDispatch(destination, source) \
    _Generic((destination).Pointer, \
        char*: MemoryCopyByte, \
        uint8_t*: MemoryCopyByte, \
        default: MemoryCopyDefault \
    )(sizeof(*(destination).Pointer), (destination).Pointer, (destination).Length, (source).Pointer, (source).Length)


#define _IS_READONLY_SPAN(span) \
        __builtin_types_compatible_p( \
                typeof((span).Pointer), \
                const typeof(*(span).Pointer) *)

#define _ASSERT_READONLY_SPAN(source) \
        static_assert(_IS_READONLY_SPAN(source), "MemoryCopy: source span must be read-only")

#define MemoryCopy(destination, source) \
    do { \
        _ASSERT_READONLY_SPAN(source); \
        _MemoryCopyDispatch((destination), (source)); \
    } while (false)

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

#define MEMORY_RESERVATION_EMPTY ((MemoryReservation){ .BaseAddress = nullptr, .PageCount = 0 })


static inline bool MemoryReservationIsEmpty(MemoryReservation memoryReservation)
{
    return memoryReservation.BaseAddress == nullptr;
}

MemoryReservation MemoryReservePages(size_t pageCount);
bool MemoryRelease(MemoryReservation* memoryReservation);

bool MemoryCommitPages(const MemoryReservation* memoryReservation, size_t pageOffset, size_t pageCount, MemoryAccess access);
bool MemoryDecommitPages(const MemoryReservation* memoryReservation, size_t pageOffset, size_t pageCount);

//---------------------------------------------------------------------------------------
// Memory Arena
//---------------------------------------------------------------------------------------

