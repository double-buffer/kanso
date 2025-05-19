#include "Test.h"

TestEntry globalTests[MAX_TESTS];
uint32_t  globalTestCount = 0;
uint32_t  globalCurrentTestIndex = 0;
char      globalTestLastErrorMessage[2048] = {0};

void RegisterTest(const char* category, const char* name, TestFunction testFunction)
{
    // TODO: We don't have a way to assert for now in the kernel
    //assert(globalTestCount < MAX_TESTS, "Too many TEST()s: increase MAX_TESTS");

    globalTests[globalTestCount++] = (TestEntry) 
    {
        .Category  = category,
        .Name = name,
        .TestFunction = testFunction
    };
}

void TestRun(TestLogHandler handler)
{
    handler(nullptr, TestHostRunState_Separator, "Running %d %s.", globalTestCount, globalTestCount == 1 ? "test" : "tests");

    for (uint32_t i = 0; i < globalTestCount; i++)
    {
        auto test = &globalTests[i];
        globalCurrentTestIndex = i;

        handler(test, TestHostRunState_Start, "");

        test->TestFunction();

        if (test->HasError)
        {
            handler(test, TestHostRunState_Error, globalTestLastErrorMessage);
        }
        else
        {
            handler(test, TestHostRunState_OK, "(xxxxns)");
        }
    }

    // TODO: Summary
}

