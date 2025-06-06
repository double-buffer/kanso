#include "Types.h"
#include "String.h"
#include "Memory.h"
#include "Version.h"
#include "Platform.h"
#include "KernelConsole.h"
#include "Kernel.h"

const char KernelLogo[] = 
{ 
    #embed "Logo.txt" 
    ,'\0'
};

__attribute__((interrupt("supervisor")))
__attribute__((section(".text.interrupt")))
void KernelSupervisorTrapHandler(CpuTrapFrame* trapFrame)
{
    auto programCounter = CpuTrapFrameGetProgramCounter(trapFrame);

    CpuClearSupervisorPendingInterrupts(CpuInterruptType_Timer);
    KernelConsolePrint(String("Kernel trap handler: %d (PC=%d).\n"), CpuReadTime(), programCounter);

    //CpuDisableSupervisorInterrupts(CpuInterruptType_Timer);
    //SbiSetTimer((uint64_t)-1);
    BiosSetTimer(CpuReadTime() + 10000000);
}

void KernelMain()
{
    auto platformInformation = PlatformGetInformation();

    KernelConsolePrint(String("\n\n\x1b[36m%s\x1b[0m\n"), KernelLogo);
    KernelConsolePrint(String("Kanso OS %s "), KANSO_VERSION_FULL);
    KernelConsolePrint(String("(%s %d-bit)\n\n"), platformInformation.Name.Pointer, platformInformation.ArchitectureBits);

    //CpuSetSupervisorTrapHandler(&KernelSupervisorTrapHandler);
    BiosSetTimer(CpuReadTime() + 10000000);
    CpuEnableSupervisorInterrupts(CpuInterruptType_Timer);

    while (true)
    {
        CpuWaitForInterrupt();
    }   
}
