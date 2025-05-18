#include "TestHost.h"
#include "KernelConsole.h"
#include "Platform.h"

// TODO: Additional flags
void KernelTestHandler(const char* message)
{
    KernelConsolePrint("Test: %s\n", message);
}

void KernelMain()
{
    TestHostRun(KernelTestHandler);
    BiosReset(BiosResetType_Shutdown, BiosResetReason_None);
}

