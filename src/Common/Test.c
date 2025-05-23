#include "Test.h"

TestEntry globalTests[MAX_TESTS];
uint32_t globalTestCount = 0;
uint32_t globalCurrentTestIndex = 0;
char globalTestLastErrorMessage[TEST_ERROR_MESSAGE_LENGTH] = {};

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
    uint32_t failedCounter = 0;

    handler(TestRunState_Separator, "Running %d %s.", globalTestCount, globalTestCount == 1 ? "test" : "tests");

    for (uint32_t i = 0; i < globalTestCount; i++)
    {
        auto test = &globalTests[i];
        globalCurrentTestIndex = i;

        handler(TestRunState_Start, "%s.%s", test->Category, test->Name);

        test->TestFunction();

        if (test->HasError)
        {
            handler(TestRunState_Failed, globalTestLastErrorMessage);
            failedCounter++;
        }
        else
        {
            handler(TestRunState_OK, "%s.%s", test->Category, test->Name);
        }
    }
    
    handler(TestRunState_Separator, "%d %s ran.", globalTestCount, globalTestCount == 1 ? "test" : "tests");
    handler(TestRunState_Passed, "%d %s.", globalTestCount - failedCounter, (globalTestCount - failedCounter) == 1 ? "test" : "tests");

    if (failedCounter > 0)
    {
        handler(TestRunState_Failed, "%d %s, listed below:", failedCounter, failedCounter == 1 ? "test" : "tests");

        for (uint32_t i = 0; i < globalTestCount; i++)
        {
            auto test = &globalTests[i];

            if (test->HasError)
            {
                handler(TestRunState_Failed, "%s.%s", test->Category, test->Name);
            }
        }
    }
}

