#include "Types.h"
#include "../../Platform.h"

struct CpuTrapFrame
{
    uintptr_t RA;
    uintptr_t SP;
    uintptr_t GP;
    uintptr_t TP;
    uintptr_t T0;
    uintptr_t T1;
    uintptr_t T2;
    uintptr_t S0;
    uintptr_t S1;
    uintptr_t A0;
    uintptr_t A1;
    uintptr_t A2;
    uintptr_t A3;
    uintptr_t A4;
    uintptr_t A5;
    uintptr_t A6;
    uintptr_t A7;
    uintptr_t S2;
    uintptr_t S3;
    uintptr_t S4;
    uintptr_t S5;
    uintptr_t S6;
    uintptr_t S7;
    uintptr_t S8;
    uintptr_t S9;
    uintptr_t S10;
    uintptr_t S11;
    uintptr_t T3;
    uintptr_t T4;
    uintptr_t T5;
    uintptr_t T6;
};

extern void kernel_trap_entry();

CpuTrapHandler globalCpuTrapHandler;

uintptr_t ComputeCpuInterruptMask(CpuInterruptType types)
{
    auto mask = 0;

    if (types & CpuInterruptType_Software)
    {
        mask |= (uintptr_t)1 << 1;
    }

    if (types & CpuInterruptType_Timer)
    {
        mask |= (uintptr_t)1 << 5;
    }

    if (types & CpuInterruptType_External)
    {
        mask |= (uintptr_t)1 << 9;
    }

    return mask;
}

#if PLATFORM_ARCHITECTURE_BITS == 32
inline uint64_t CpuReadTime()
{
    uint32_t high, low, tmp;

    __asm__ volatile (
        ".Lread_time_%=:\n"
        "rdtimeh %0\n"
        "rdtime  %1\n"
        "rdtimeh %2\n"
        "bne     %0, %2, .Lread_time_%="
        : "=&r"(high), "=&r"(low), "=&r"(tmp));

    return ((uint64_t)high << 32) | low;
}

inline uint64_t CpuReadCycle()
{
    uint32_t high, low, tmp;

    __asm__ volatile (
        ".Lread_cycle_%=:\n"
        "rdcycleh %0\n"
        "rdcycle  %1\n"
        "rdcycleh %2\n"
        "bne      %0, %2, .Lread_cycle_%="
        : "=&r"(high), "=&r"(low), "=&r"(tmp));

    return ((uint64_t)high << 32) | low;
}
#else
inline uint64_t CpuReadTime()
{
    auto result = 0UL;
    __asm__ volatile("rdtime %0" : "=r"(result));
    return result;
}

inline uint64_t CpuReadCycle()
{
    auto result = 0UL;
    __asm__ volatile("rdcycle %0" : "=r"(result));
    return result;
}
#endif

inline void CpuSetTrapHandler(CpuTrapHandler trapHandler)
{
    globalCpuTrapHandler = trapHandler;

    if (trapHandler)
    {
        __asm__ volatile(
          "csrw stvec, %0\n"
          "csrsi sstatus, 2"
          : 
          : "r" (kernel_trap_entry));
    }
    else
    {
        __asm__ volatile("csrsi sstatus, 0");
    }
}

inline void CpuEnableInterrupts(CpuInterruptType types)
{
    auto mask = ComputeCpuInterruptMask(types);

    __asm__ volatile (
        "csrs sie, %0"
        :
        : "r"(mask)
        : "memory"
    );
}

inline void CpuDisableInterrupts(CpuInterruptType types)
{
    auto mask = ComputeCpuInterruptMask(types);

    __asm__ volatile (
        "csrc sie, %0"
        :
        : "r"(mask)
        : "memory"
    );
}

inline void CpuClearPendingInterrupts(CpuInterruptType types)
{
    auto mask = ComputeCpuInterruptMask(types);

    __asm__ volatile (
        "csrc sip, %0"
        :
        : "r"(mask)
        : "memory"
    );
}

inline void CpuWaitForInterrupt()
{
    __asm__ __volatile__("wfi");
}

inline uintptr_t CpuTrapFrameGetProgramCounter(const CpuTrapFrame* trapFrame)
{
    // TODO: Move that to ConsoleLogTrapFrame
    KernelConsolePrint(String("raw ptr = %x\n"), trapFrame);

    KernelConsolePrint(String("RA: %x\n"), trapFrame->RA);
    KernelConsolePrint(String("SP: %x\n"), trapFrame->SP);
    KernelConsolePrint(String("GP: %x\n"), trapFrame->GP);
    KernelConsolePrint(String("TP: %x\n"), trapFrame->TP);
    KernelConsolePrint(String("T0: %x\n"), trapFrame->T0);
    KernelConsolePrint(String("T1: %x\n"), trapFrame->T1);
    KernelConsolePrint(String("T2: %x\n"), trapFrame->T2);
    KernelConsolePrint(String("S0: %x\n"), trapFrame->S0);
    KernelConsolePrint(String("S1: %x\n"), trapFrame->S1);
    KernelConsolePrint(String("A0: %x\n"), trapFrame->A0);

    return trapFrame->T0;
}
