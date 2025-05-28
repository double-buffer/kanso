#include "../../Platform.h"
#include "String.h"

PlatformInformation PlatformGetInformation()
{
    return (PlatformInformation)
    {
        .Name = String("RISC-V"),
        .ArchitectureBits = PLATFORM_ARCHITECTURE_BITS
    };
}

