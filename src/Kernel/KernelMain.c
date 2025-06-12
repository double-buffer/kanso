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

void KernelTrapHandler(CpuTrapFrame* trapFrame)
{
    CpuLogTrapFrame(trapFrame);
    auto trapCause = CpuTrapFrameGetCause(trapFrame);

    if (trapCause.Type == CpuTrapCauseType_Interrupt)
    {
        switch (trapCause.InterruptType)
        {
            case CpuInterruptType_Timer:
                CpuClearPendingInterrupts(CpuInterruptType_Timer);
                //CpuDisableInterrupts(CpuInterruptType_Timer);
                //SbiSetTimer((uint64_t)-1);
                BiosSetTimer(CpuReadTime() + 10000000);
                
                auto programCounter = CpuTrapFrameGetProgramCounter(trapFrame);
                KernelConsolePrint(String("Kernel trap handler: %l (PC=%x).\n"), CpuReadTime(), programCounter);

                return;

            default:
                KernelFailure(String("Unknown interrupt type. (InterruptType: %x)"), trapCause.InterruptType);
        }
    }

    KernelFailure(String("Unknown Kernel Trap Cause. (Cause=%x)"), trapCause.Type);
}

void KernelMain()
{
    auto platformInformation = PlatformGetInformation();

    KernelConsolePrint(String("\n\n\x1b[36m%s\x1b[0m\n"), KernelLogo);
    KernelConsolePrint(String("Kanso OS %s "), KANSO_VERSION_FULL);
    KernelConsolePrint(String("(%s %d-bit)\n\n"), platformInformation.Name.Pointer, platformInformation.ArchitectureBits);

    CpuSetTrapHandler(KernelTrapHandler);
    BiosSetTimer(CpuReadTime() + 10000000);
    CpuEnableInterrupts(CpuInterruptType_Timer);

    while (true)
    {
        CpuGenerateInvalidInstruction();
        CpuWaitForInterrupt();
    }   
}
