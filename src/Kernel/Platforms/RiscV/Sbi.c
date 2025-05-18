#include "Types.h"
#include "../../Platform.h"

// Doc on SBI is available here: https://github.com/riscv-non-isa/riscv-sbi-doc/tree/master

typedef enum : int64_t
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
    int64_t Value;
} SbiReturn;

SbiReturn SbiCallFunction(uint64_t extensionId, uint64_t functionId, 
                     uint64_t parameter0, uint64_t parameter1, uint64_t parameter2,
                     uint64_t parameter3, uint64_t parameter4, uint64_t parameter5)
{
    register uint64_t a0 asm ("a0") = parameter0;
    register uint64_t a1 asm ("a1") = parameter1;
    register uint64_t a2 asm ("a2") = parameter2;
    register uint64_t a3 asm ("a3") = parameter3;
    register uint64_t a4 asm ("a4") = parameter4;
    register uint64_t a5 asm ("a5") = parameter5;
    register uint64_t a6 asm ("a6") = functionId;
    register uint64_t a7 asm ("a7") = extensionId;

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

void BiosDebugConsoleWrite(const char* message, uint32_t length)
{
    SbiCallFunction(SbiExtension_DebugConsole, 0x00, length, (uint64_t)message, 0, 0, 0, 0);
}

void BiosSetTimer(uint64_t timeValue)
{
    SbiCallFunction(SbiExtension_Time, 0x00, timeValue, 0, 0, 0, 0, 0);
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
