#include "Types.h"
#include "../../Platform.h"

struct CpuTrapFrame
{
    uintptr_t PC;
};

extern void supervisor_trap_entry();

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
          : "r" (supervisor_trap_entry));
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
    return trapFrame->PC;
}
