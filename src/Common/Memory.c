#include "Memory.h"

// TODO: This will need to be thread local
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
