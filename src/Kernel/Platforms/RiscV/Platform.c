#include "../../Platform.h"
#include "Memory.h"
#include "String.h"
#include "Types.h"

// TODO: Add tests

uintptr_t globalBootHartId;
uintptr_t globalDeviceTreeData;

// TODO: Merge get devices into one function. But maybe GetInformation is not great
// because we retrieve the whole device map also
PlatformInformation PlatformGetInformation()
{
    return (PlatformInformation)
    {
        .Name = String("RISC-V"),
        .ArchitectureBits = PLATFORM_ARCHITECTURE_BITS,
        .BootCpuId = globalBootHartId
    };
}

// TODO: Put that in a common binary reader or similar and do tests
// TODO: We should have a create binary reader that take the encoding and the pointer
// TODO: It would be cool if it could work with streams but it seems too much
// TODO: This function should take into account the endianness of the platform
uint32_t ConvertBytesToUint32(ReadOnlySpanUint8 data, ByteOrder byteOrder)
{
    // TODO: Check length is at least 4
    // TODO: For now big endian -> little endian conversion
    auto result = *(uint32_t*)data.Pointer;

    if (PLATFORM_BYTE_ORDER != byteOrder)
    {
        result = __builtin_bswap32(result);
    }

    return result;
}

typedef struct
{
    ReadOnlySpanUint8 Data;
    ByteOrder ByteOrder; 
    size_t CurrentOffset;
} BinaryReader;

BinaryReader CreateBinaryReader(ReadOnlySpanUint8 data, ByteOrder byteOrder)
{
    return (BinaryReader)
    {
        .Data = data,
        .ByteOrder = byteOrder,
        .CurrentOffset = 0
    };
}

uint32_t BinaryReadUint32(BinaryReader* reader)
{
    auto span = SpanSliceFrom(reader->Data, reader->CurrentOffset);
    reader->CurrentOffset += sizeof(uint32_t);

    return ConvertBytesToUint32(span, reader->ByteOrder);
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
    
    while (span.Pointer[length] != '\0')
    {
        output->Pointer[length] = span.Pointer[length];
        length++;
    }

    output->Length = length;
    reader->CurrentOffset += length + 1;
}

void BinarySetOffset(BinaryReader* reader, size_t offset)
{
    reader->CurrentOffset = offset;
}

bool DeviceTreeReadNode(BinaryReader* reader, size_t stringDataOffset)
{
    auto testNode = BinaryReadUint32(reader);

    if (testNode == 0x01)
    {
        auto name = StackAllocChar(1024);
        BinaryReadString(reader, &name);
        BinarySetOffset(reader, MemoryAlignUp(reader->CurrentOffset, 4));

        KernelConsolePrint(String("BeginNode: '%s'\n"), name);
    }
    else if (testNode == 0x02)
    {
        KernelConsolePrint(String("EndNode.\n"));
    }
    else if (testNode == 0x03)
    {
        auto length = BinaryReadUint32(reader);
        auto nameOffset = BinaryReadUint32(reader);

        auto value = StackAllocUint8(1024);
        BinaryReadBytes(reader, length, &value);

        auto offset = reader->CurrentOffset;
        BinarySetOffset(reader, stringDataOffset + nameOffset);

        auto name = StackAllocChar(1024);
        BinaryReadString(reader, &name);

        BinarySetOffset(reader, MemoryAlignUp(offset, 4));
        KernelConsolePrint(String("  Property: %s\n"), name);
    }
    else if (testNode == 0x09)
    {
        return false;
    }

    return true;
}

PlatformDevices PlatformGetDevices()
{
    auto dtbHeaderData = CreateReadOnlySpanUint8((uint8_t*)globalDeviceTreeData, sizeof(uint32_t) * 2);

    auto dtbMagic = ConvertBytesToUint32(dtbHeaderData, ByteOrder_BigEndian);
    auto sizeInBytes = ConvertBytesToUint32(SpanSliceFrom(dtbHeaderData, sizeof(uint32_t)), ByteOrder_BigEndian);

    KernelConsolePrint(String("MagicDTB: %x\n"), dtbMagic);
    // TODO: Check magic
    // TODO: Verify version
    
    auto dataSpan = CreateReadOnlySpanUint8((const uint8_t*)globalDeviceTreeData, sizeInBytes);
    auto reader = CreateBinaryReader(dataSpan, ByteOrder_BigEndian);
    BinarySetOffset(&reader, sizeof(uint32_t) * 2);

    auto structureOffset = BinaryReadUint32(&reader);
    auto stringDataOffset = BinaryReadUint32(&reader);

    // TODO: Parse the rest of the header

    BinarySetOffset(&reader, structureOffset);

    while (DeviceTreeReadNode(&reader, stringDataOffset))
    {
    }

    // TODO: We parse DTB here for now but it will be moved in Kernel/Devices/DTB
    return (PlatformDevices)
    {
    };
}
