#include "Types.h"
#include "../../Platform.h"

uint64_t ComputeCpuInterruptMask(CpuInterruptType types)
{
    auto mask = 0ul;

    if (types & CpuInterruptType_Software)
    {
        mask |= (1ul << 1);
    }

    if (types & CpuInterruptType_Timer)
    {
        mask |= (1ul << 5);
    }

    if (types & CpuInterruptType_External)
    {
        mask |= (1ul << 9);
    }

    return mask;
}

inline uint64_t CpuReadTime()
{
    auto result = 0ul;
    __asm__ volatile("rdtime %0" : "=r" (result));
    return result;
}

inline uint64_t CpuReadCycle()
{
    auto result = 0ul;
    __asm__ volatile("rdcycle %0" : "=r" (result));
    return result;
}

inline void CpuSetSupervisorTrapHandler(CpuTrapHandler trapHandler)
{
    __asm__ volatile(
      "csrw stvec, %0\n"
      "csrsi sstatus, 2"
      : 
      : "r" (trapHandler));
}

inline void CpuEnableSupervisorInterrupts(CpuInterruptType types)
{
    auto mask = ComputeCpuInterruptMask(types);

    __asm__ volatile (
        "csrs sie, %0"
        :
        : "r"(mask)
        : "memory"
    );
}

inline void CpuDisableSupervisorInterrupts(CpuInterruptType types)
{
    auto mask = ComputeCpuInterruptMask(types);

    __asm__ volatile (
        "csrc sie, %0"
        :
        : "r"(mask)
        : "memory"
    );
}

inline void CpuClearSupervisorPendingInterrupts(CpuInterruptType types)
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

