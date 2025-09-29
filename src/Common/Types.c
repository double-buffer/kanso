#include "Types.h"

// TODO: This will need to be thread local
TypeError globalTypeError = TypeError_None;

size_t BitArrayFindFirstNotSet(BitArray bitArray)
{
    if (BitArrayIsEmpty(bitArray)) 
    {
        globalTypeError = TypeError_InvalidParameter;
        return SIZE_MAX;
    }

    for (uint32_t i = 0; i < bitArray.Data.Length; i++)
    {
        auto inverse = ~bitArray.Data.Pointer[i];

        if (inverse == 0)
        {
            continue;
        }

        globalTypeError = TypeError_None;
        auto result = SizePrefixCountZeros(inverse);

        return (i * BITS_PER_SIZE_TYPE) + result;
    }

    globalTypeError = TypeError_NotFound;
    return SIZE_MAX;
}

size_t BitArrayFindRangeNotSet(BitArray bitArray, size_t length)
{
    if (BitArrayIsEmpty(bitArray) || length == 0 || length > bitArray.BitCount) 
    {
        globalTypeError = TypeError_InvalidParameter;
        return SIZE_MAX;
    }

    size_t result = 0;
    size_t currentCount = 0;

    for (uint32_t i = 0; i < bitArray.BitCount; i++)
    {
        if (!BitArrayIsSet(bitArray, i))
        {
            if (currentCount == 0)
            {
                result = i;
            }

            currentCount++;

            if (currentCount == length)
            {
                globalTypeError = TypeError_None;
                return result;
            }
        }
        else 
        {
            currentCount = 0;
        }
    }

    globalTypeError = TypeError_NotFound;
    return SIZE_MAX;
}
