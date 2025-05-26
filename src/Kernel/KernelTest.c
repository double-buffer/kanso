#include "Memory.h"
#include "Test.h"
#include "String.h"
#include "KernelConsole.h"
#include "Platform.h"

const char* TEST_CONSOLE_RESET = "\x1b[0m";
const char* TEST_CONSOLE_GREEN = "\x1b[32m";
const char* TEST_CONSOLE_RED = "\x1b[31m";

void KernelTestHandler(TestRunState state, ReadOnlySpanChar message, ...)
{
    if (state == TestRunState_Start)
    {
        KernelConsolePrint(String("%s[ RUN      ]%s"), TEST_CONSOLE_GREEN, TEST_CONSOLE_RESET);
    }
    else if (state == TestRunState_OK)
    {
        KernelConsolePrint(String("%s[       OK ]%s"), TEST_CONSOLE_GREEN, TEST_CONSOLE_RESET);
    }
    else if (state == TestRunState_Passed)
    {
        KernelConsolePrint(String("%s[  PASSED  ]%s"), TEST_CONSOLE_GREEN, TEST_CONSOLE_RESET);
    }
    else if (state == TestRunState_Failed)
    {
        KernelConsolePrint(String("%s[  FAILED  ]%s"), TEST_CONSOLE_RED, TEST_CONSOLE_RESET);
    }
    else if (state == TestRunState_Separator)
    {
        KernelConsolePrint(String("%s[==========]%s"), TEST_CONSOLE_GREEN, TEST_CONSOLE_RESET);
    }

    va_list vargs;
    va_start(vargs, message);

    auto tmp = StackAllocChar(256);
    StringFormatVargs(&tmp, message, vargs);

    KernelConsolePrint(String(" %s\n"), tmp);

    va_end(vargs);
}

void KernelMain()
{
    TestRun(KernelTestHandler);
    BiosReset(BiosResetType_Shutdown, BiosResetReason_None);
}

