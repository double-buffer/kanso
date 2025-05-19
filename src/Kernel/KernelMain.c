#include "Types.h"
#include "Memory.h"
#include "Platform.h"
#include "KernelConsole.h"

const char KernelLogo[] = 
{ 
    #embed "Logo.txt" 
    ,'\0'
};

__attribute__((interrupt("supervisor")))
__attribute__((section(".text.interrupt")))
void KernelSupervisorTrapHandler()
{
    CpuClearSupervisorPendingInterrupts(CpuInterruptType_Timer);
    KernelConsolePrint("Kernel trap handler: %d.\n", CpuReadTime());

    //CpuDisableSupervisorInterrupts(CpuInterruptType_Timer);
    //SbiSetTimer((uint64_t)-1);
    BiosSetTimer(CpuReadTime() + 10000000);
}

void KernelMain()
{
    KernelConsolePrint("\n\n%s\nKanso OS 1.0-DEV1\n\n", KernelLogo);

    CpuSetSupervisorTrapHandler(&KernelSupervisorTrapHandler);
    BiosSetTimer(CpuReadTime() + 10000000);
    CpuEnableSupervisorInterrupts(CpuInterruptType_Timer);

    while (true)
    {
        CpuWaitForInterrupt();
    }   
}
