#include "Memory.h"

void MemorySetUint8(void* destination, size_t length, size_t stride, const void* value)
{
    (void)stride;
    uint8_t* pointer = destination;
    uint8_t byteValue = *(uint8_t*)value;

    for (size_t i = 0; i < length; i++)
    {
        pointer[i] = byteValue;
    }
}

void MemorySetDefault(void* destination, size_t length, size_t stride, const void* value)
{
    uint8_t* pointer = destination;

    for (size_t i = 0; i < length; i++)
    {
        for (size_t j = 0; j < stride; j++)
        {
            pointer[(i * stride) + j] = ((uint8_t*)value)[j];
        }
    }
}

void MemoryCopyUint8(void* destination, size_t destinationLength, size_t stride, const void* source, size_t sourceLength)
{
    (void)stride;

    for (size_t i = 0; i < sourceLength; i++)
    {
        ((uint8_t*)destination)[i] = ((uint8_t*)source)[i];
    }
}

void MemoryCopyDefault(void* destination, size_t destinationLength, size_t stride, const void* source, size_t sourceLength)
{
    uint8_t* pointer = destination;

    for (size_t i = 0; i < sourceLength; i++)
    {
        for (size_t j = 0; j < stride; j++)
        {
            pointer[(i * stride) + j] = ((uint8_t*)source)[(i * stride) + j];
        }
    }
}

// TODO: Move that to the standard library
void memset(uint8_t* destination, uint8_t value, size_t sizeInBytes) 
{
    MemorySetUint8(destination, sizeInBytes, sizeof(uint8_t), &value);
}
