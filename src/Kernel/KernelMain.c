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
    KernelConsolePrint(String("Trap Handler\n"));
    auto trapCause = CpuTrapFrameGetCause(trapFrame);
    auto errorName = String("Unknown kernel trap cause");

    if (trapCause.Type == CpuTrapType_Interrupt)
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
                errorName = String("Unknown interrupt type");
        }
    }
    else
    {
        switch (trapCause.SynchronousType)
        {
            case CpuTrapSynchronousType_InstructionError:
                errorName = String("Instruction error");
                break;

            case CpuTrapSynchronousType_AddressError:
                errorName = String("Address error");
                break;

            case CpuTrapSynchronousType_Debug:
                errorName = String("Debug not implemented");
                break;

            case CpuTrapSynchronousType_PageError:
                errorName = String("Page error");
                break;

            case CpuTrapSynchronousType_IntegrityError:
                errorName = String("Integrity error");
                break;

            case CpuTrapSynchronousType_HardwareError:
                errorName = String("Hardware error");
                break;

            default:
                errorName = String("Unknown synchronous trap type");
        }
    }

    CpuLogTrapFrame(trapFrame);
    KernelFailure(String("%s. (Code=%x, Extra=%x)"), errorName, trapCause.Code, trapCause.ExtraInformation);
}
static void DumpCSRs(const char *tag)
{
    uintptr_t sstatus, sie, sip;
    __asm__ volatile ("csrr %0, sstatus" : "=r"(sstatus));
    __asm__ volatile ("csrr %0, sie"     : "=r"(sie));
    __asm__ volatile ("csrr %0, sip"     : "=r"(sip));
    KernelConsolePrint(String("[%s] sstatus=%x  sie=%x  sip=%x\n"),
                       tag, sstatus, sie, sip);
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

    KernelConsolePrint(String("Set Trap handler\n"));
    CpuSetTrapHandler(KernelTrapHandler);
    KernelConsolePrint(String("Set Timer\n"));

    KernelConsolePrint(String("Enable Interrupts\n"));
    CpuEnableInterrupts(CpuInterruptType_All);
    DumpCSRs("after enable");

    //BiosSetTimer(CpuReadTime() + 10'000'000);
    DumpCSRs("after set-timer");

    KernelConsolePrint(String("Entering loop\n"));

    while (true)
    {
        KernelConsolePrint(String("Loop WFI\n"));


        __asm__ volatile ("csrs sip, %0" :: "r"(1UL << 1)); 
        //CpuGenerateInvalidInstruction();
        CpuWaitForInterrupt();
    }   
}
