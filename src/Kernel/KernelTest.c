#include "Test.h"
#include "String.h"
#include "KernelConsole.h"
#include "Platform.h"

const char* TEST_CONSOLE_RESET = "\x1b[0m";
const char* TEST_CONSOLE_GREEN = "\x1b[32m";
const char* TEST_CONSOLE_RED = "\x1b[31m";

void KernelTestHandler(TestRunState state, const char* message, ...)
{
    if (state == TestRunState_Start)
    {
        KernelConsolePrint("%s[ RUN      ]%s", TEST_CONSOLE_GREEN, TEST_CONSOLE_RESET);
    }
    else if (state == TestRunState_OK)
    {
        KernelConsolePrint("%s[       OK ]%s", TEST_CONSOLE_GREEN, TEST_CONSOLE_RESET);
    }
    else if (state == TestRunState_Passed)
    {
        KernelConsolePrint("%s[  PASSED  ]%s", TEST_CONSOLE_GREEN, TEST_CONSOLE_RESET);
    }
    else if (state == TestRunState_Failed)
    {
        KernelConsolePrint("%s[  FAILED  ]%s", TEST_CONSOLE_RED, TEST_CONSOLE_RESET);
    }
    else if (state == TestRunState_Separator)
    {
        KernelConsolePrint("%s[==========]%s", TEST_CONSOLE_GREEN, TEST_CONSOLE_RESET);
    }

    va_list vargs;
    va_start(vargs, message);

    char tmp[256] = {};
    uint32_t tmpLength = 0;
    StringFormatVA(tmp, &tmpLength, message, vargs);

    KernelConsolePrint(" %s\n", tmp);

    va_end(vargs);
}

void KernelMain()
{
    TestRun(KernelTestHandler);
    BiosReset(BiosResetType_Shutdown, BiosResetReason_None);
}

