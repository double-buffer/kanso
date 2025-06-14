#include "Memory.h"
#include "Test.h"
#include "String.h"
#include "KernelConsole.h"
#include "Platform.h"
#include "Version.h"

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

void KernelMain()
{
    auto platformInformation = PlatformGetInformation();

    KernelConsoleSetForegroundColor(KernelConsoleColorHighlight);
    KernelConsolePrint(String("\n\nKanso OS Kernel Tests %s "), KANSO_VERSION_FULL);
    KernelConsolePrint(String("(%s %d-bit)\n\n"), platformInformation.Name.Pointer, platformInformation.ArchitectureBits);
    KernelConsoleResetStyle();

    TestRun(KernelTestHandler);
    BiosReset(BiosResetType_Shutdown, BiosResetReason_None);
}

