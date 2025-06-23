#include "Test.h"
#include "Memory.h"

TestEntry globalTests[MAX_TESTS];
uint32_t globalTestCount = 0;
uint32_t globalCurrentTestIndex = 0;
SpanChar globalTestLastErrorMessage = {};

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

void TestRun(TestLogHandler handler, ReadOnlySpanChar categoryFilters)
{
    uint32_t failedCounter = 0;
    globalTestLastErrorMessage = StackAllocChar(TEST_ERROR_MESSAGE_LENGTH);

    handler(TestRunState_Separator, String("Running %d %s."), globalTestCount, globalTestCount == 1 ? "test" : "tests");

    for (uint32_t i = 0; i < globalTestCount; i++)
    {
        auto test = &globalTests[i];
        globalCurrentTestIndex = i;

        if (categoryFilters.Length > 0)
        {
            // TODO: Split
            for (uint32_t j = 0; j < categoryFilters.Length; j++)
            {
        handler(TestRunState_Start, String("Checking Category: %s.%s"), test->Category, test->Name);
            }
        }

        handler(TestRunState_Start, String("%s.%s"), test->Category, test->Name);

        test->TestFunction();

        if (test->HasError)
        {
            handler(TestRunState_Failed, ToReadOnlySpanChar(globalTestLastErrorMessage));
            failedCounter++;
        }
        else
        {
            handler(TestRunState_OK, String("%s.%s"), test->Category, test->Name);
        }
    }
    
    handler(TestRunState_Separator, String("%d %s ran."), globalTestCount, globalTestCount == 1 ? "test" : "tests");
    handler(TestRunState_Passed, String("%d %s."), globalTestCount - failedCounter, (globalTestCount - failedCounter) == 1 ? "test" : "tests");

    if (failedCounter > 0)
    {
        handler(TestRunState_Failed, String("%d %s, listed below:"), failedCounter, failedCounter == 1 ? "test" : "tests");

        for (uint32_t i = 0; i < globalTestCount; i++)
        {
            auto test = &globalTests[i];

            if (test->HasError)
            {
                handler(TestRunState_Failed, String("%s.%s"), test->Category, test->Name);
            }
        }
    }
}

