#pragma once

typedef void (*TestHostHandler)(const char*);

void TestHostRun(TestHostHandler handler);
