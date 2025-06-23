#include "Test.h"
#include "Memory.h"
#include "String.h"

TestEntry globalTests[MAX_TESTS];
uint32_t globalTestCount = 0;
uint32_t globalCurrentTestIndex = 0;

// TODO: Do someting better here?
char globalTestLastErrorMessageStorage[TEST_ERROR_MESSAGE_LENGTH];
SpanChar globalTestLastErrorMessage = { .Pointer = globalTestLastErrorMessageStorage, .Length = TEST_ERROR_MESSAGE_LENGTH };

void RegisterTest(ReadOnlySpanChar category, ReadOnlySpanChar name, TestFunction testFunction)
{
    // TODO: We don't have a way to assert for now in the kernel
    //assert(globalTestCount < MAX_TESTS, "Too many TEST()s: increase MAX_TESTS");

    globalTests[globalTestCount++] = (TestEntry) 
    {
        .Category  = category,
        .Name = name,
        .TestFunction = testFunction,
    };
}

void TestRun(TestLogHandler handler, ReadOnlySpanChar categoryFilters)
{
    uint32_t testRunCount = 0;

    for (uint32_t i = 0; i < globalTestCount; i++)
    {
        auto test = &globalTests[i];
        test->CanRun = true;

        if (categoryFilters.Length > 0)
        {
            auto splittedFilters = StackAllocString(64);
            StringSplit(&splittedFilters, categoryFilters, '|');

            auto testCanRun = false;

            for (uint32_t j = 0; j < splittedFilters.Length; j++)
            {
                if (StringEquals(test->Category, SpanAt(splittedFilters, j)))
                {
                    testCanRun = true;
                }
            }
            
            test->CanRun = testCanRun;
        }

        if (test->CanRun)
        {
            testRunCount++;
        }
    }

    uint32_t failedCounter = 0;

    handler(TestRunState_Separator, String("Running %d %s."), testRunCount, testRunCount == 1 ? "test" : "tests");

    for (uint32_t i = 0; i < globalTestCount; i++)
    {
        auto test = &globalTests[i];
        test->HasError = false;
        globalCurrentTestIndex = i;

        handler(TestRunState_Start, String("%s.%s"), test->Category.Pointer, test->Name.Pointer);

        test->TestFunction();

        if (test->HasError)
        {
            handler(TestRunState_Failed, ToReadOnlySpanChar(globalTestLastErrorMessage));
            failedCounter++;
        }
        else
        {
            handler(TestRunState_OK, String("%s.%s"), test->Category.Pointer, test->Name.Pointer);
        }
    }
    
    handler(TestRunState_Separator, String("%d %s ran."), testRunCount, testRunCount == 1 ? "test" : "tests");
    handler(TestRunState_Passed, String("%d %s."), testRunCount - failedCounter, (testRunCount - failedCounter) == 1 ? "test" : "tests");

    if (failedCounter > 0)
    {
        handler(TestRunState_Failed, String("%d %s, listed below:"), failedCounter, failedCounter == 1 ? "test" : "tests");

        for (uint32_t i = 0; i < globalTestCount; i++)
        {
            auto test = &globalTests[i];

            if (test->HasError)
            {
                handler(TestRunState_Failed, String("%s.%s"), test->Category.Pointer, test->Name.Pointer);
            }
        }
    }
}

