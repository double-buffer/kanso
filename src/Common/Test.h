#pragma once

#include "Memory.h"
#include "Types.h"
#include "String.h"

#define MAX_TESTS 1024
#define TEST_ERROR_MESSAGE_LENGTH 2048

typedef enum
{
    TestRunState_Start,
    TestRunState_OK,
    TestRunState_Passed,
    TestRunState_Failed,
    TestRunState_Separator,
} TestRunState;

typedef void (*TestFunction)();
typedef void (*TestLogHandler)(TestRunState state, ReadOnlySpanChar message, ...);

typedef struct 
{
    const char* Category;
    const char* Name;
    TestFunction TestFunction;
    bool HasError;
} TestEntry;


extern TestEntry globalTests[MAX_TESTS];
extern uint32_t globalTestCount;
extern uint32_t globalCurrentTestIndex;
extern SpanChar globalTestLastErrorMessage;


#define Test(category, name) \
    static void test_##category##_##name(); \
    \
    [[gnu::constructor]] \
    static void __register_##category##_##name() \
    { \
        RegisterTest(#category, #name, test_##category##_##name); \
    } \
    \
    static void test_##category##_##name() \


#define TestAssertCore(expr, expected, actual, operator) \
    do { \
        if (!(expr)) { \
            TestEntry* testEntry = &globalTests[globalCurrentTestIndex]; \
            testEntry->HasError = true; \
            StringFormat(&globalTestLastErrorMessage, String("%s\n  Expected: %s\n    Actual: %d %s %d"), __FILE__, #expr, expected, operator, actual); \
            return; \
        } \
    } while (false)

#define TestAssertEquals(expected, actual) TestAssertCore((expected) == (actual), expected, actual, "==")
#define TestAssertNotEquals(expected, actual) TestAssertCore((expected) != (actual), expected, actual, "!=")

#define TestAssertStringEquals(expected, actual) \
    do { \
        if (finalString.Length != destination.Length) \
        { \
            TestEntry* testEntry = &globalTests[globalCurrentTestIndex]; \
            testEntry->HasError = true; \
            StringFormat(&globalTestLastErrorMessage, String("%s\n  Expected: (%s.Length) == (%s.Length)\n    Actual: %d == %d"), __FILE__, #expected, #actual, expected.Length, actual.Length); \
            return; \
        } \
        auto containsErrors = false; \
        \ 
        for (uint32_t i = 0; i < actual.Length; i++) \
        { \
            if (actual.Pointer[i] != expected.Pointer[i]) \
            { \
                containsErrors = true; \
                break; \
            } \
        } \
        \
        if (containsErrors) \
        { \
            TestEntry* testEntry = &globalTests[globalCurrentTestIndex]; \
            testEntry->HasError = true; \
            StringFormat(&globalTestLastErrorMessage, String("%s\n  Expected: (%s) == (%s)\n    Actual: \"%s\" == \"%s\""), __FILE__, #expected, #actual, expected.Pointer, actual.Pointer); \
            return; \
        } \
    } while (false)

void RegisterTest(const char* category, const char* name, TestFunction testFunction);
void TestRun(TestLogHandler handler);
