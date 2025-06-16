#include "../../Platform.h"
#include "Memory.h"
#include "String.h"
#include "Types.h"

// TODO: Add tests

uintptr_t globalBootHartId;
uintptr_t globalDeviceTreeData;

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
uint32_t BinaryReadUint32Old(uintptr_t pointer)
{
   // TODO: For now big endian -> little endian conversion
    auto result = *(uint32_t*)pointer;

    if (PLATFORM_BYTE_ORDER == ByteOrder_LittleEndian)
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
    // TODO: For now big endian -> little endian conversion
    // TODO: Do the other conversions
    auto result = *(uint32_t*)span.Pointer;

    if (PLATFORM_BYTE_ORDER == ByteOrder_LittleEndian)
    {
        result = __builtin_bswap32(result);
    }

    reader->CurrentOffset += sizeof(uint32_t);

    return result;
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

void DeviceTreeReadNode(BinaryReader* reader, size_t stringDataOffset)
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
        KernelConsolePrint(String("EndNode\n"));
    }
    else if (testNode == 0x03)
    {
        auto length = BinaryReadUint32(reader);
        auto nameOffset = BinaryReadUint32(reader);

        if (length != 4)
        {
            KernelConsolePrint(String("  Unknown length: %d\n"), length);
        }

        auto value = BinaryReadUint32(reader);

        auto offset = reader->CurrentOffset;
        BinarySetOffset(reader, stringDataOffset + nameOffset);

        auto name = StackAllocChar(1024);
        BinaryReadString(reader, &name);

        BinarySetOffset(reader, MemoryAlignUp(offset, 4));
        KernelConsolePrint(String("  Property: %s, %d\n"), name, value);
    }
    else   
    {
        KernelConsolePrint(String("Unknown Node: %d\n"), testNode);
    }
}

PlatformDevices PlatformGetDevices()
{
    auto dtbMagic = BinaryReadUint32Old(globalDeviceTreeData);
    auto sizeInBytes = BinaryReadUint32Old(globalDeviceTreeData + sizeof(uint32_t));

    // TODO: Check magic
    // TODO: Check boot_cpuid_phys
    
    auto dataSpan = CreateReadOnlySpanUint8((const uint8_t*)globalDeviceTreeData, sizeInBytes);
    auto reader = CreateBinaryReader(dataSpan, ByteOrder_BigEndian);
    BinarySetOffset(&reader, sizeof(uint32_t) * 2);

    auto structureOffset = BinaryReadUint32(&reader);
    auto stringDataOffset = BinaryReadUint32(&reader);

    // TODO: Parse the rest of the header

    BinarySetOffset(&reader, structureOffset);

    DeviceTreeReadNode(&reader, stringDataOffset);
    DeviceTreeReadNode(&reader, stringDataOffset);
    DeviceTreeReadNode(&reader, stringDataOffset);

    DeviceTreeReadNode(&reader, stringDataOffset);
    DeviceTreeReadNode(&reader, stringDataOffset);
    DeviceTreeReadNode(&reader, stringDataOffset);


    /* This is a property node parsing
    testNode = BinaryReadUint32(&reader);
    length = BinaryReadUint32(&reader);
    nameOffset = BinaryReadUint32(&reader);
    KernelConsolePrint(String("TestNode: %d, %d, %d\n"), testNode, length, nameOffset);
    auto test = SpanSliceFrom(dataSpan, stringDataOffset + nameOffset);
    KernelConsolePrint(String("Test: %s\n"), test);
    */


    // TODO: We parse DTB here for now but it will be moved in Kernel/Devices/DTB
    return (PlatformDevices)
    {
    };
}
