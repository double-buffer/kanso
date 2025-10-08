#include "BinaryReader.h"
#include "Memory.h"

BinaryReader CreateBinaryReader(ReadOnlySpanUint8 data, ByteOrder byteOrder)
{
    return (BinaryReader)
    {
        .Data = data,
        .ByteOrder = byteOrder,
        .CurrentOffset = 0
    };
}

void BinarySetOffset(BinaryReader* reader, size_t offset)
{
    reader->CurrentOffset = offset;
}

uint8_t BinaryReadUint8(BinaryReader* reader)
{
    return 0;
}

uint32_t BinaryReadUint32(BinaryReader* reader)
{
    auto span = SpanSliceFrom(reader->Data, reader->CurrentOffset);
    reader->CurrentOffset += sizeof(uint32_t);

    return ConvertBytesToUint32(span, reader->ByteOrder);
}

uint64_t BinaryReadUint64(BinaryReader* reader)
{
    auto span = SpanSliceFrom(reader->Data, reader->CurrentOffset);
    reader->CurrentOffset += sizeof(uint64_t);

    return ConvertBytesToUint64(span, reader->ByteOrder);
}

// TODO: When we have memoryarena we can maybe do better
void BinaryReadBytes(BinaryReader* reader, size_t length, SpanUint8* output)
{
    auto span = SpanSlice(reader->Data, reader->CurrentOffset, length);
    // TODO: For now big endian -> little endian conversion
    // TODO: Do the other conversions

    MemoryCopy(*output, span);

    output->Length = length;
    reader->CurrentOffset += length;
}

// TODO: When we have memoryarena we can maybe do better
void BinaryReadString(BinaryReader* reader, SpanChar* output)
{
    auto span = SpanSliceFrom(reader->Data, reader->CurrentOffset);
    // TODO: For now big endian -> little endian conversion
    // TODO: Do the other conversions

    uint32_t length = 0;
    
    while (SpanAt(span, length) != '\0')
    {
        SpanAt(*output, length) = SpanAt(span, length);
        length++;
    }

    output->Length = length;
    reader->CurrentOffset += length + 1;
}
