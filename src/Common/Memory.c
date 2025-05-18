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
            pointer[i * stride + j] = ((uint8_t*)value)[j];
        }
    }
}

// TODO: Move that to the standard library
void memset(uint8_t* destination, uint8_t value, size_t sizeInBytes) 
{
    MemorySetUint8(destination, sizeInBytes, sizeof(uint8_t), &value);
}
