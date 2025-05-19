#include "Test.h"
#include "String.h"
#include "KernelConsole.h"
#include "Platform.h"

const char* TEST_CONSOLE_RESET = "\x1b[0m";
const char* TEST_CONSOLE_GREEN = "\x1b[32m";
const char* TEST_CONSOLE_RED = "\x1b[31m";

extern void (*__INIT_ARRAY_START[])(void);
extern void (*__INIT_ARRAY_END[])(void);

static void run_constructors(void) {
    for (void (**ctor)(void) = __INIT_ARRAY_START;
         ctor < __INIT_ARRAY_END;
         ++ctor)
    {
        (*ctor)();
    }
}

void KernelTestHandler(const TestEntry* test, TestHostRunState state, const char* message, ...)
{
    __builtin_va_list vargs;
    __builtin_va_start(vargs, message);

    if (state == TestHostRunState_Start)
    {
        KernelConsolePrint("%s[ RUN      ]%s", TEST_CONSOLE_GREEN, TEST_CONSOLE_RESET);
    }
    else if (state == TestHostRunState_OK)
    {
        KernelConsolePrint("%s[       OK ]%s", TEST_CONSOLE_GREEN, TEST_CONSOLE_RESET);
    }
    else if (state == TestHostRunState_Error)
    {
        KernelConsolePrint("%s[  FAILED  ]%s", TEST_CONSOLE_RED, TEST_CONSOLE_RESET);
    }
    else if (state == TestHostRunState_Separator)
    {
        KernelConsolePrint("%s[==========]%s", TEST_CONSOLE_GREEN, TEST_CONSOLE_RESET);
    }

    if (test)
    {
        KernelConsolePrint(" %s.%s %s\n", test->Category, test->Name, message);
    }
    else
    {
        char tmp[256] = {};
        uint32_t tmpLength = 0;
        StringFormatVA(tmp, &tmpLength, message, vargs);

        KernelConsolePrint(" %s\n", tmp);
    }

    __builtin_va_end(vargs);
}

void KernelMain()
{
    run_constructors();
    TestRun(KernelTestHandler);
    BiosReset(BiosResetType_Shutdown, BiosResetReason_None);
}

