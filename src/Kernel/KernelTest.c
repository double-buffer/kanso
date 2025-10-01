#include "Kanso.h"
#include "Kernel.h"
#include "Platform.h"

void KernelTrapHandler(CpuTrapFrame* trapFrame)
{
    auto trapCause = CpuTrapFrameGetCause(trapFrame);
    auto errorName = String("Unknown kernel trap cause");

    if (trapCause.Type == CpuTrapType_Synchronous)
    {
        switch (trapCause.SynchronousType)
        {
            case CpuTrapSynchronousType_InstructionError:
                errorName = String("Instruction error");
                break;

            case CpuTrapSynchronousType_AddressError:
                errorName = String("Address error");
                break;

            case CpuTrapSynchronousType_Debug:
                errorName = String("Debug not implemented");
                break;

            case CpuTrapSynchronousType_PageError:
                errorName = String("Page error");
                break;

            case CpuTrapSynchronousType_IntegrityError:
                errorName = String("Integrity error");
                break;

            case CpuTrapSynchronousType_HardwareError:
                errorName = String("Hardware error");
                break;

            default:
                errorName = String("Unknown synchronous trap type");
        }
    }

    CpuLogTrapFrame(trapFrame);
    KernelFailure(String("%s. (Code=%x, Extra=%x)"), errorName, trapCause.Code, trapCause.ExtraInformation);
}

// TODO: Move that out of the way from the kernel. It will be the same code in user mode.
void KernelTestHandler(TestRunState state, ReadOnlySpanChar message, ...)
{
    if (state == TestRunState_Start)
    {
        ConsoleSetForegroundColor(ConsoleColorSuccess);
        ConsolePrint(String("[ RUN      ]"));
    }
    else if (state == TestRunState_OK)
    {
        ConsoleSetForegroundColor(ConsoleColorSuccess);
        ConsolePrint(String("[       OK ]"));
    }
    else if (state == TestRunState_Passed)
    {
        ConsoleSetForegroundColor(ConsoleColorSuccess);
        ConsolePrint(String("[  PASSED  ]"));
    }
    else if (state == TestRunState_Failed)
    {
        ConsoleSetForegroundColor(ConsoleColorError);
        ConsolePrint(String("[  FAILED  ]"));
    }
    else if (state == TestRunState_Separator)
    {
        ConsoleSetForegroundColor(ConsoleColorSuccess);
        ConsolePrint(String("[==========]"));
    }

    ConsoleResetStyle();

    va_list vargs;
    va_start(vargs, message);

    StackMemoryArena(stackMemoryArena);
    auto tmp = StringFormatVargs(stackMemoryArena, message, vargs);

    ConsolePrint(String(" %s\n"), tmp);

    va_end(vargs);
}

void KernelInit()
{
    CpuSetTrapHandler(KernelTrapHandler);
    
    auto platformInformation = PlatformGetInformation();

    ConsoleSetForegroundColor(ConsoleColorHighlight);
    ConsolePrint(String("\n\nKanso OS KernelInit Tests %s "), KANSO_VERSION_FULL);
    ConsolePrint(String("(%s %d-bit)\n\n"), platformInformation.SystemInformation.Name.Pointer, platformInformation.SystemInformation.ArchitectureBits);
    ConsoleResetStyle();

    TestRun(KernelTestHandler, String("Types|Memory"));

    // TODO: Init the MMU

}

void KernelMain()
{
    auto platformInformation = PlatformGetInformation();

    ConsoleSetForegroundColor(ConsoleColorHighlight);
    ConsolePrint(String("\n\nKanso OS KernelMain Tests %s "), KANSO_VERSION_FULL);
    ConsolePrint(String("(%s %d-bit)\n\n"), platformInformation.SystemInformation.Name.Pointer, platformInformation.SystemInformation.ArchitectureBits);
    ConsoleResetStyle();

    //TestRun(KernelTestHandler, String(""));

    BiosReset(BiosResetType_Shutdown, BiosResetReason_None);
}

