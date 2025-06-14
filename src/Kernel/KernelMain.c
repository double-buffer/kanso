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
                KernelFailure(String("Unknown interrupt type. (Code=%x, Extra=%x)"), trapCause.Code, trapCause.ExtraInformation);
        }
    }

    KernelFailure(String("Unknown kernel trap cause. (Code=%x, Extra=%x)"), trapCause.Code, trapCause.ExtraInformation);
}

void KernelMain()
{
    auto platformInformation = PlatformGetInformation();

    KernelConsoleSetForegroundColor(KernelConsoleColorAccent);
    KernelConsolePrint(String("\n\n%s\n"), KernelLogo);
    KernelConsoleResetStyle();

    KernelConsoleSetForegroundColor(KernelConsoleColorHighlight);
    KernelConsolePrint(String("Kanso OS %s "), KANSO_VERSION_FULL);
    KernelConsolePrint(String("(%s %d-bit)\n\n"), platformInformation.Name.Pointer, platformInformation.ArchitectureBits);
    KernelConsoleResetStyle();

    CpuSetTrapHandler(KernelTrapHandler);
    BiosSetTimer(CpuReadTime() + 10000000);
    CpuEnableInterrupts(CpuInterruptType_Timer);

    while (true)
    {
        CpuGenerateInvalidInstruction();
        CpuWaitForInterrupt();
    }   
}
