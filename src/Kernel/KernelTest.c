#include "Memory.h"
#include "Test.h"
#include "String.h"
#include "KernelConsole.h"
#include "Kernel.h"
#include "Platform.h"
#include "Version.h"

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

void KernelTestHandler(TestRunState state, ReadOnlySpanChar message, ...)
{
    if (state == TestRunState_Start)
    {
        KernelConsoleSetForegroundColor(KernelConsoleColorSuccess);
        KernelConsolePrint(String("[ RUN      ]"));
    }
    else if (state == TestRunState_OK)
    {
        KernelConsoleSetForegroundColor(KernelConsoleColorSuccess);
        KernelConsolePrint(String("[       OK ]"));
    }
    else if (state == TestRunState_Passed)
    {
        KernelConsoleSetForegroundColor(KernelConsoleColorSuccess);
        KernelConsolePrint(String("[  PASSED  ]"));
    }
    else if (state == TestRunState_Failed)
    {
        KernelConsoleSetForegroundColor(KernelConsoleColorError);
        KernelConsolePrint(String("[  FAILED  ]"));
    }
    else if (state == TestRunState_Separator)
    {
        KernelConsoleSetForegroundColor(KernelConsoleColorSuccess);
        KernelConsolePrint(String("[==========]"));
    }

    KernelConsoleResetStyle();

    va_list vargs;
    va_start(vargs, message);

    auto tmp = StackAllocChar(256);
    StringFormatVargs(&tmp, message, vargs);

    KernelConsolePrint(String(" %s\n"), tmp);

    va_end(vargs);
}

void KernelInit()
{
    CpuSetTrapHandler(KernelTrapHandler);
    
    auto platformInformation = PlatformGetInformation();

    KernelConsoleSetForegroundColor(KernelConsoleColorHighlight);
    KernelConsolePrint(String("\n\nKanso OS KernelInit Tests %s "), KANSO_VERSION_FULL);
    KernelConsolePrint(String("(%s %d-bit)\n\n"), platformInformation.SystemInformation.Name.Pointer, platformInformation.SystemInformation.ArchitectureBits);
    KernelConsoleResetStyle();

    TestRun(KernelTestHandler, String("Types|Memory"));
}

void KernelMain()
{
    auto platformInformation = PlatformGetInformation();

    KernelConsoleSetForegroundColor(KernelConsoleColorHighlight);
    KernelConsolePrint(String("\n\nKanso OS KernelMain Tests %s "), KANSO_VERSION_FULL);
    KernelConsolePrint(String("(%s %d-bit)\n\n"), platformInformation.SystemInformation.Name.Pointer, platformInformation.SystemInformation.ArchitectureBits);
    KernelConsoleResetStyle();

    //TestRun(KernelTestHandler, String(""));
    BiosReset(BiosResetType_Shutdown, BiosResetReason_None);
}

