#include "../../Platform.h"

#define RISCV_MEMORY_PAGESIZE 4096

// TODO: Add tests
extern uint8_t __INIT_HEAP_START[];
extern uint8_t __INIT_HEAP_END[];

uintptr_t globalBootHartId;
uintptr_t globalDeviceTreeData;

PlatformInformation globalPlatformInformation = {};

PlatformDevices PlatformGetDevices();

// TODO: Merge get devices into one function. But maybe GetInformation is not great
// because we retrieve the whole device map also
PlatformInformation PlatformGetInformation()
{
    if (globalPlatformInformation.SystemInformation.ArchitectureBits == 0)
    {
        globalPlatformInformation = (PlatformInformation)
        {
            .SystemInformation = 
            {
                .Name = String("RISC-V"),
                .ArchitectureBits = PLATFORM_ARCHITECTURE_BITS,
                .PageSize = RISCV_MEMORY_PAGESIZE
            },
            .BootCpuId = globalBootHartId,
            .InitHeap = CreateSpan(uint8_t, __INIT_HEAP_START, __INIT_HEAP_END - __INIT_HEAP_START)
        };

        // TODO: Remove, this is just a test
        PlatformGetDevices();
    }

    return globalPlatformInformation;
}

bool DeviceTreeReadNode(BinaryReader* reader, size_t stringDataOffset)
{
    auto testNode = BinaryReadUint32(reader);

    if (testNode == 0x01)
    {
        auto name = StackAlloc(char, 1024);
        BinaryReadString(reader, &name);
        BinarySetOffset(reader, AlignUp(reader->CurrentOffset, 4));

        ConsolePrint(String("BeginNode: '%s'\n"), name);
    }
    else if (testNode == 0x02)
    {
        ConsolePrint(String("EndNode.\n"));
    }
    else if (testNode == 0x03)
    {
        auto length = BinaryReadUint32(reader);
        auto nameOffset = BinaryReadUint32(reader);

        auto value = StackAlloc(uint8_t, 1024);
        BinaryReadBytes(reader, length, &value);

        auto offset = reader->CurrentOffset;
        BinarySetOffset(reader, stringDataOffset + nameOffset);

        auto name = StackAlloc(char, 1024);
        BinaryReadString(reader, &name);

        BinarySetOffset(reader, AlignUp(offset, 4));
        ConsolePrint(String("  Property: %s\n"), name);
    }
    else if (testNode == 0x09)
    {
        return false;
    }

    return true;
}

PlatformDevices PlatformGetDevices()
{
    auto dtbHeaderData = CreateReadOnlySpan(uint8_t, (uint8_t*)globalDeviceTreeData, sizeof(uint32_t) * 2);

    auto dtbMagic = ConvertBytesToUint32(dtbHeaderData, ByteOrder_BigEndian);
    auto sizeInBytes = ConvertBytesToUint32(SpanSliceFrom(dtbHeaderData, sizeof(uint32_t)), ByteOrder_BigEndian);

    ConsolePrint(String("MagicDTB: %x\n"), dtbMagic);
    // TODO: Check magic
    // TODO: Verify version

    auto dataSpan = CreateReadOnlySpan(uint8_t, (uint8_t*)globalDeviceTreeData, sizeInBytes);
    auto reader = CreateBinaryReader(dataSpan, ByteOrder_BigEndian);
    BinarySetOffset(&reader, sizeof(uint32_t) * 2);

    auto structureOffset = BinaryReadUint32(&reader);
    auto stringDataOffset = BinaryReadUint32(&reader);
    auto reservedMemoryDataOffset = BinaryReadUint32(&reader);

    // TODO: Parse the rest of the header
    BinarySetOffset(&reader, reservedMemoryDataOffset);

    uint64_t reservedOffset = 1;
    uint64_t reservedSize = 1;

    while (reservedOffset != 0 && reservedSize != 0)
    {
        reservedOffset = BinaryReadUint64(&reader);
        reservedSize = BinaryReadUint64(&reader);

        if (reservedOffset != 0 && reservedSize != 0)
        {
            ConsolePrint(String("Reserved Memory: %x (size: %x)\n"), reservedOffset, reservedSize);
        }
    }


    BinarySetOffset(&reader, structureOffset);

    while (DeviceTreeReadNode(&reader, stringDataOffset))
    {
    }

    // TODO: We parse DTB here for now but it will be moved in Kernel/Devices/DTB
    return (PlatformDevices)
    {
    };
}
