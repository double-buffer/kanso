#include "Types.h"

// TODO: This will need to be thread local
TypeError globalTypeError = TypeError_None;

size_t BitArrayFindFirstNotSet(BitArray bitArray)
{
    if (BitArrayIsEmpty(bitArray)) 
    {
        globalTypeError = TypeError_InvalidParameter;
        // TODO: Return invalid size_t -> SIZE_MAX
        return 0;
    }

    for (uint32_t i = 0; i < bitArray.Data.Length; i++)
    {
        auto inverse = ~bitArray.Data.Pointer[i];

        if (inverse == 0)
        {
            continue;
        }

        // TODO: Rename
        auto result = CoutTrailingZeros(inverse);
        return i * BITS_PER_SIZE_TYPE + result;
    }

    return 0;
}
