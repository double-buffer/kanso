#include "Types.h"
#include "../../Platform.h"

// Doc on SBI is available here: https://github.com/riscv-non-isa/riscv-sbi-doc/tree/master

typedef enum : intptr_t
{
    SbiReturnCode_Success = 0,
    SbiReturnCode_ErrorFailed = -1,
    SbiReturnCode_ErrorNotSupported = -2,
    SbiReturnCode_ErrorInvalidParameter = -3,
    SbiReturnCode_ErrorDenied = -4,
    SbiReturnCode_ErrorInvalidAddress = -5,
    SbiReturnCode_ErrorAlreadyAvailable = -6,
    SbiReturnCode_ErrorAlreadyStarted = -7,
    SbiReturnCode_ErrorAlreadyStopped = -8,
    SbiReturnCode_ErrorNoSharedMemory = -9,
    SbiReturnCode_ErrorInvalidState = -10,
    SbiReturnCode_ErrorBadRange = -11,
    SbiReturnCode_ErrorTimeout = -12,
    SbiReturnCode_ErrorIO = -13,
    SbiReturnCode_ErrorDeniedLocked = -14
} SbiReturnCode;

typedef enum
{
    SbiExtension_DebugConsole = 0x4442434E,
    SbiExtension_Time = 0x54494D45,
    SbiExtension_SystemReset = 0x53525354
} SbiExtension;

typedef struct
{
    SbiReturnCode ReturnCode;
    intptr_t Value;
} SbiReturn;

SbiReturn SbiCallFunction(uintptr_t extensionId, uintptr_t functionId, 
                     uintptr_t parameter0, uintptr_t parameter1, uintptr_t parameter2,
                     uintptr_t parameter3, uintptr_t parameter4, uintptr_t parameter5)
{
    register uintptr_t a0 asm ("a0") = parameter0;
    register uintptr_t a1 asm ("a1") = parameter1;
    register uintptr_t a2 asm ("a2") = parameter2;
    register uintptr_t a3 asm ("a3") = parameter3;
    register uintptr_t a4 asm ("a4") = parameter4;
    register uintptr_t a5 asm ("a5") = parameter5;
    register uintptr_t a6 asm ("a6") = functionId;
    register uintptr_t a7 asm ("a7") = extensionId;

    __asm__ __volatile__(
        "ecall" 
        : "+r" (a0), "+r" (a1)
        : "r" (a2), "r" (a3), "r" (a4), "r" (a5), "r" (a6), "r" (a7)
        : "memory");

    return (SbiReturn)
    {
        .ReturnCode = a0,
        .Value = a1
    };
}

void BiosDebugConsoleWrite(ReadOnlySpanChar message)
{
    SbiCallFunction(SbiExtension_DebugConsole, 0x00, message.Length, (uintptr_t)message.Pointer, 0, 0, 0, 0);
}

void BiosSetTimer(uint64_t timeValue)
{
#if PLATFORM_ARCHITECTURE_BITS == 32
    SbiCallFunction(SbiExtension_Time, 0x00, (uintptr_t)timeValue, (uintptr_t)(timeValue >> 32), 0, 0, 0, 0);
#else
    SbiCallFunction(SbiExtension_Time, 0x00, timeValue, 0, 0, 0, 0, 0);
#endif
}

void BiosReset(BiosResetType resetType, BiosResetReason reason)
{
    auto sbiResetType = 0x00000000;

    if (resetType == BiosResetType_ColdReboot)
    {
        sbiResetType = 0x00000001;
    }
    else if (resetType == BiosResetType_ColdReboot)
    {
        sbiResetType = 0x00000002;
    }

    auto sbiReason = 0x00000000;

    if (reason == BiosResetReason_SystemFailure)
    {
        sbiReason = 0x00000001;
    }

    SbiCallFunction(SbiExtension_SystemReset, sbiResetType, sbiReason, 0, 0, 0, 0, 0);
}
