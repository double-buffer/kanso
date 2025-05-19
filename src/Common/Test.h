#pragma once

#include "Types.h"

#define MAX_TESTS 1024

typedef void (*TestFunction)();

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
extern char globalTestLastErrorMessage[2048];

void RegisterTest(const char* category, const char* name, TestFunction testFunction);

// TODO: The constructor stuff is really ugly
#define Test(category, name) \
    static void test_##category##_##name(); \
    \
    static void __attribute__((constructor)) \
    __register_##category##_##name(void) \
    { \
        RegisterTest(#category, #name, test_##category##_##name); \
    } \
    \
    static void test_##category##_##name() \


#define TestAssert(expr)                                \
    do {                                            \
        if (!(expr)) {                              \
            TestEntry *t = &globalTests[globalCurrentTestIndex]; \
            t->HasError = true; \
            uint32_t messageLength; \
            StringFormat(globalTestLastErrorMessage, &messageLength, "    ? %s.%s: `%s` failed\n", t->Category, t->Name, #expr);                  \
            return;                                 \
        }                                           \
    } while (false)

typedef enum
{
    TestHostRunState_Start,
    TestHostRunState_OK,
    TestHostRunState_Error,
    TestHostRunState_Separator
} TestHostRunState;

typedef void (*TestLogHandler)(const TestEntry* test, TestHostRunState state, const char* message, ...);

void TestRun(TestLogHandler handler);
