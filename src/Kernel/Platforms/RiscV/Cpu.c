#include "Types.h"
#include "../../KernelConsole.h"
#include "../../Platform.h"

#define RISCV_INTERRUPT_SOFTWARE 1
#define RISCV_INTERRUPT_TIMER 5
#define RISCV_INTERRUPT_EXTERNAL 9

typedef struct 
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
} GeneralPurposeRegisters;

typedef struct
{
    uintptr_t Epc;
    uintptr_t Status;
    uintptr_t Scratch;
    uintptr_t Cause;
    uintptr_t TrapValue;
} SupervisorRegisters;

struct CpuTrapFrame
{
    // TODO: Add a TrapFrameType to specify if it is partial, full, float, vector, etc.
    GeneralPurposeRegisters GeneralPurposeRegisters;
    SupervisorRegisters SupervisorRegisters;
};

static_assert(sizeof(CpuTrapFrame) % 16 == 0);

extern void kernel_trap_entry();

CpuTrapHandler globalCpuTrapHandler;

uintptr_t ComputeCpuInterruptMask(CpuInterruptType types)
{
    auto mask = 0;

    if (types & CpuInterruptType_Software)
    {
        mask |= (uintptr_t)1 << RISCV_INTERRUPT_SOFTWARE;
    }

    if (types & CpuInterruptType_Timer)
    {
        mask |= (uintptr_t)1 << RISCV_INTERRUPT_TIMER;
    }

    if (types & CpuInterruptType_External)
    {
        mask |= (uintptr_t)1 << RISCV_INTERRUPT_EXTERNAL;
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

inline void CpuGenerateInvalidInstruction()
{
    __asm__ volatile ("unimp");
}

/* Masks and “all-ones” patterns for the three fields we test. */
#define RV_MASK_LOW2        0x0003u   /* bits[1:0] */
#define RV_MASK_MID3        0x001Cu   /* bits[4:2] */
#define RV_MASK_HIGH2       0x0060u   /* bits[6:5] */

#define RV_PATTERN_LOW2_32  0x0003u   /* 0b11 */
#define RV_PATTERN_MID3_48  0x001Cu   /* 0b111 << 2 */
#define RV_PATTERN_HIGH2_64 0x0060u   /* 0b11  << 5 */

/* Resulting instruction sizes in bytes. */
#define RV_SIZE_16          2u
#define RV_SIZE_32          4u
#define RV_SIZE_48          6u
#define RV_SIZE_64          8u

uintptr_t CpuComputeNextInstructionAddress(uintptr_t instructionAddress)
{
    /* Text might be in an execute-only segment; cast via unsigned char avoids UB. */
    const uint16_t firstHalfword =
        *(const uint16_t *)(const unsigned char *)instructionAddress;

    /* --- 1. 16-bit? -------------------------------------------------------- */
    if ((firstHalfword & RV_MASK_LOW2) != RV_PATTERN_LOW2_32)
        return instructionAddress + RV_SIZE_16;

    /* --- 2. 32-bit? -------------------------------------------------------- */
    if ((firstHalfword & RV_MASK_MID3) != RV_PATTERN_MID3_48)
        return instructionAddress + RV_SIZE_32;

    /* --- 3. 48-bit? -------------------------------------------------------- */
    if ((firstHalfword & RV_MASK_HIGH2) != RV_PATTERN_HIGH2_64)
        return instructionAddress + RV_SIZE_48;

    /* --- 4. Otherwise it must be 64-bit ----------------------------------- */
    return instructionAddress + RV_SIZE_64;
}

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

void LogGeneralPurposeRegisters(const GeneralPurposeRegisters* generalPurposeRegisters)
{
    KernelConsolePrint(String("ra:  %x    sp:  %x    gp:  %x\n"), generalPurposeRegisters->RA, generalPurposeRegisters->SP, generalPurposeRegisters->GP);
    KernelConsolePrint(String("tp:  %x    t0:  %x    t1:  %x\n"), generalPurposeRegisters->TP, generalPurposeRegisters->T0, generalPurposeRegisters->T1);
    KernelConsolePrint(String("t2:  %x    s0:  %x    s1:  %x\n"), generalPurposeRegisters->T2, generalPurposeRegisters->S0, generalPurposeRegisters->S1);
    KernelConsolePrint(String("a0:  %x    a1:  %x    a2:  %x\n"), generalPurposeRegisters->A0, generalPurposeRegisters->A1, generalPurposeRegisters->A2);
    KernelConsolePrint(String("a3:  %x    a4:  %x    a5:  %x\n"), generalPurposeRegisters->A3, generalPurposeRegisters->A4, generalPurposeRegisters->A5);
    KernelConsolePrint(String("a6:  %x    a7:  %x    s2:  %x\n"), generalPurposeRegisters->A6, generalPurposeRegisters->A7, generalPurposeRegisters->S2);
    KernelConsolePrint(String("s3:  %x    s4:  %x    s5:  %x\n"), generalPurposeRegisters->S3, generalPurposeRegisters->S4, generalPurposeRegisters->S5);
    KernelConsolePrint(String("s6:  %x    s7:  %x    s8:  %x\n"), generalPurposeRegisters->S6, generalPurposeRegisters->S7, generalPurposeRegisters->S8);
    KernelConsolePrint(String("s9:  %x    s10: %x    s11: %x\n"), generalPurposeRegisters->S9, generalPurposeRegisters->S10, generalPurposeRegisters->S11);
    KernelConsolePrint(String("t3:  %x    t4:  %x    t5:  %x\n"), generalPurposeRegisters->T3, generalPurposeRegisters->T4, generalPurposeRegisters->T5);
    KernelConsolePrint(String("t6:  %x\n"), generalPurposeRegisters->T6);
}

void LogSupervisorRegisters(const SupervisorRegisters* supervisorRegisters)
{
    KernelConsolePrint(String("sepc:   %x    sstatus: %x    sscratch: %x\n"), supervisorRegisters->Epc, supervisorRegisters->Status, supervisorRegisters->Scratch);
    KernelConsolePrint(String("scause: %x    stval:   %x\n"), supervisorRegisters->Cause, supervisorRegisters->TrapValue);
}

void CpuLogTrapFrame(const CpuTrapFrame* trapFrame)
{
    KernelConsolePrint(String("Trap Frame:\n"));
    KernelConsolePrint(String("===========\n\n"));

    KernelConsolePrint(String("General Purpose Registers:\n"));
    LogGeneralPurposeRegisters(&trapFrame->GeneralPurposeRegisters);

    KernelConsolePrint(String("\nSupervisor Registers:\n"));
    LogSupervisorRegisters(&trapFrame->SupervisorRegisters);
}

CpuTrapCause CpuTrapFrameGetCause(const CpuTrapFrame* trapFrame)
{
    auto isInterrupt = (trapFrame->SupervisorRegisters.Cause >> (sizeof(uintptr_t) * 8 - 1) > 0);
    auto causeCode = trapFrame->SupervisorRegisters.Cause & ((1ULL<<((sizeof(uintptr_t)*8)-1))-1);

    auto interruptType = CpuInterruptType_None;

    if (isInterrupt)
    {
        // TODO: Extract constants
        switch (causeCode)
        {
            case RISCV_INTERRUPT_SOFTWARE:
                interruptType = CpuInterruptType_Software;
                break;

            case RISCV_INTERRUPT_TIMER:
                interruptType = CpuInterruptType_Timer;
                break;

            case RISCV_INTERRUPT_EXTERNAL:
                interruptType = CpuInterruptType_External;
                break;
        }
    }

    return (CpuTrapCause)
    {
        .Type = isInterrupt ? CpuTrapCauseType_Interrupt : CpuTrapCauseType_Unknown,
        .InterruptType = interruptType
    };

    /*
    switch (code) {
        case 8:  return TC_SYSCALL;
        case 12: return TC_PAGE_FAULT_INS;
        case 13: return TC_PAGE_FAULT_LOAD;
        case 15: return TC_PAGE_FAULT_STORE;
        case 2:  return TC_ILLEGAL_INSTRUCTION;
        default: return TC_OTHER;
    }*/
}

inline uintptr_t CpuTrapFrameGetProgramCounter(const CpuTrapFrame* trapFrame)
{
    return trapFrame->SupervisorRegisters.Epc;
}

void CpuTrapFrameSetProgramCounter(CpuTrapFrame* trapFrame, uintptr_t value)
{
    trapFrame->SupervisorRegisters.Epc = value;
}
