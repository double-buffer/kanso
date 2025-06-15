#include "Memory.h"
#include "Types.h"
#include "../../KernelConsole.h"
#include "../../Platform.h"

#define RISCV_INTERRUPT_SOFTWARE 1
#define RISCV_INTERRUPT_TIMER 5
#define RISCV_INTERRUPT_EXTERNAL 9

#define RISCV_TRAP_SYNCHRONOUS_INSTRUCTION_ADDRESS_MISALIGNED 0 
#define RISCV_TRAP_SYNCHRONOUS_INSTRUCTION_ACCESS_FAULT 1 
#define RISCV_TRAP_SYNCHRONOUS_ILLEGAL_INSTRUCTION 2 
#define RISCV_TRAP_SYNCHRONOUS_BREAKPOINT 3 
#define RISCV_TRAP_LOAD_ADDRESS_MISALIGNED 4 
#define RISCV_TRAP_LOAD_ACCESS_FAULT 5 
#define RISCV_TRAP_STORE_AMO_ADDRESS_MISALIGNED 6 
#define RISCV_TRAP_STORE_AMO_ACCESS_FAULT 7 
#define RISCV_TRAP_ECALL_FROM_USER_MODE 8 
#define RISCV_TRAP_ECALL_FROM_SUPERVISOR_MODE 9 
#define RISCV_TRAP_INSTRUCTION_PAGE_FAULT 12 
#define RISCV_TRAP_LOAD_PAGE_FAULT 13 
#define RISCV_TRAP_STORE_AMO_PAGE_FAULT 15 
#define RISCV_TRAP_SOFTWARE_CHECK 18 
#define RISCV_TRAP_HARDWARE_ERROR 19 

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
    //__asm__ volatile (".word 0xFFFFFFFF");
    __asm__ volatile ("auipc t0, 0 \njalr x0, t0, 2");
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
          "csrw sie,  zero\n"
          "csrci sstatus, 2\n" 
          "csrw stvec, %0"
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
        "csrs sie, %0\n"
        "csrsi sstatus, 2"
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
    __asm__ __volatile__("wfi" ::: "memory");
}

void LogRegister(ReadOnlySpanChar name, uintptr_t value, uint8_t padding, bool insertTab)
{
    // TODO: Color the register name with the keyword blue of VScode?
    KernelConsoleSetForegroundColor(KernelConsoleColorKeyword);
    KernelConsolePrint(String("%s"), name);
    KernelConsoleResetStyle();

    KernelConsolePrint(String(":"));

    for (uint32_t i = 0; i < padding; i++)
    {
        KernelConsolePrint(String(" "));
    }

    KernelConsoleSetForegroundColor(KernelConsoleColorNumeric);
    KernelConsolePrint(String("%x"), value);
    KernelConsoleResetStyle();

    if (insertTab)
    {
        KernelConsolePrint(String("    "));
    }
    else
    {
        KernelConsolePrint(String("\n"));
    }
}

void LogGeneralPurposeRegisters(const GeneralPurposeRegisters* generalPurposeRegisters)
{
    LogRegister(String("ra"), generalPurposeRegisters->RA, 2, true);
    LogRegister(String("sp"), generalPurposeRegisters->SP, 2, true);
    LogRegister(String("gp"), generalPurposeRegisters->GP, 2, false);

    LogRegister(String("tp"), generalPurposeRegisters->TP, 2, true);
    LogRegister(String("t0"), generalPurposeRegisters->T0, 2, true);
    LogRegister(String("t1"), generalPurposeRegisters->T1, 2, false);

    LogRegister(String("t2"), generalPurposeRegisters->T2, 2, true);
    LogRegister(String("s0"), generalPurposeRegisters->S0, 2, true);
    LogRegister(String("s1"), generalPurposeRegisters->S1, 2, false);

    LogRegister(String("a0"), generalPurposeRegisters->A0, 2, true);
    LogRegister(String("a1"), generalPurposeRegisters->A1, 2, true);
    LogRegister(String("a2"), generalPurposeRegisters->A2, 2, false);

    LogRegister(String("a3"), generalPurposeRegisters->A3, 2, true);
    LogRegister(String("a4"), generalPurposeRegisters->A4, 2, true);
    LogRegister(String("a5"), generalPurposeRegisters->A5, 2, false);

    LogRegister(String("a6"), generalPurposeRegisters->A6, 2, true);
    LogRegister(String("a7"), generalPurposeRegisters->A7, 2, true);
    LogRegister(String("s2"), generalPurposeRegisters->S2, 2, false);

    LogRegister(String("s3"), generalPurposeRegisters->S3, 2, true);
    LogRegister(String("s4"), generalPurposeRegisters->S4, 2, true);
    LogRegister(String("s5"), generalPurposeRegisters->S5, 2, false);

    LogRegister(String("s6"), generalPurposeRegisters->S6, 2, true);
    LogRegister(String("s7"), generalPurposeRegisters->S7, 2, true);
    LogRegister(String("s8"), generalPurposeRegisters->S8, 2, false);

    LogRegister(String("s9"), generalPurposeRegisters->S9, 2, true);
    LogRegister(String("s10"), generalPurposeRegisters->S10, 1, true);
    LogRegister(String("s11"), generalPurposeRegisters->S11, 1, false);

    LogRegister(String("t3"), generalPurposeRegisters->T3, 2, true);
    LogRegister(String("t4"), generalPurposeRegisters->T4, 2, true);
    LogRegister(String("t5"), generalPurposeRegisters->T5, 2, false);

    LogRegister(String("t6"), generalPurposeRegisters->T6, 2, false);
}

void LogSupervisorRegisters(const SupervisorRegisters* supervisorRegisters)
{
    LogRegister(String("sepc"), supervisorRegisters->Epc, 3, true);
    LogRegister(String("sstatus"), supervisorRegisters->Status, 1, true);
    LogRegister(String("sscratch"), supervisorRegisters->Scratch, 1, false);

    LogRegister(String("scause"), supervisorRegisters->Cause, 1, true);
    LogRegister(String("stval"), supervisorRegisters->TrapValue, 3, false);
}

void CpuLogTrapFrame(const CpuTrapFrame* trapFrame)
{
    KernelConsoleSetForegroundColor(KernelConsoleColorInfo);
    KernelConsolePrintBoxMessage(String("Trap Frame"));
    KernelConsoleResetStyle();

    KernelConsoleSetForegroundColor(KernelConsoleColorHighlight);
    KernelConsolePrint(String("General Purpose Registers:\n"));
    KernelConsoleResetStyle();
    LogGeneralPurposeRegisters(&trapFrame->GeneralPurposeRegisters);

    KernelConsoleSetForegroundColor(KernelConsoleColorHighlight);
    KernelConsolePrint(String("\nSupervisor Registers:\n"));
    KernelConsoleResetStyle();
    LogSupervisorRegisters(&trapFrame->SupervisorRegisters);
}

CpuTrapCause CpuTrapFrameGetCause(const CpuTrapFrame* trapFrame)
{
    auto isInterrupt = (trapFrame->SupervisorRegisters.Cause >> (sizeof(uintptr_t) * 8 - 1) > 0);
    auto causeCode = trapFrame->SupervisorRegisters.Cause & ((1ULL<<((sizeof(uintptr_t)*8)-1))-1);

    auto interruptType = CpuInterruptType_None;
    auto synchronousType = CpuTrapSynchronousType_Unknown;

    if (isInterrupt)
    {
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
    
    else
    {
        switch (causeCode)
        {
            case RISCV_TRAP_SYNCHRONOUS_INSTRUCTION_ADDRESS_MISALIGNED:
            case RISCV_TRAP_SYNCHRONOUS_INSTRUCTION_ACCESS_FAULT:
            case RISCV_TRAP_SYNCHRONOUS_ILLEGAL_INSTRUCTION:
                synchronousType = CpuTrapSynchronousType_InstructionError;
                break;

            case RISCV_TRAP_SYNCHRONOUS_BREAKPOINT:
                synchronousType = CpuTrapSynchronousType_Debug;
                break;

            case RISCV_TRAP_LOAD_ADDRESS_MISALIGNED: 
            case RISCV_TRAP_LOAD_ACCESS_FAULT: 
            case RISCV_TRAP_STORE_AMO_ADDRESS_MISALIGNED: 
            case RISCV_TRAP_STORE_AMO_ACCESS_FAULT: 
                synchronousType = CpuTrapSynchronousType_AddressError;
                break;

            case RISCV_TRAP_ECALL_FROM_USER_MODE: 
            case RISCV_TRAP_ECALL_FROM_SUPERVISOR_MODE: 
                synchronousType = CpuTrapSynchronousType_SystemCall;
                break;

            case RISCV_TRAP_INSTRUCTION_PAGE_FAULT: 
            case RISCV_TRAP_LOAD_PAGE_FAULT: 
            case RISCV_TRAP_STORE_AMO_PAGE_FAULT: 
                synchronousType = CpuTrapSynchronousType_PageError;
                break;

            case RISCV_TRAP_SOFTWARE_CHECK: 
                synchronousType = CpuTrapSynchronousType_IntegrityError;
                break;

            case RISCV_TRAP_HARDWARE_ERROR: 
                synchronousType = CpuTrapSynchronousType_HardwareError;
                break;
        }
    }

    return (CpuTrapCause)
    {
        .Type = isInterrupt ? CpuTrapType_Interrupt : CpuTrapType_Synchronous,
        .InterruptType = interruptType,
        .SynchronousType = synchronousType,
        .Code = trapFrame->SupervisorRegisters.Cause,
        .ExtraInformation = trapFrame->SupervisorRegisters.TrapValue
    };
}

inline uintptr_t CpuTrapFrameGetProgramCounter(const CpuTrapFrame* trapFrame)
{
    return trapFrame->SupervisorRegisters.Epc;
}

void CpuTrapFrameSetProgramCounter(CpuTrapFrame* trapFrame, uintptr_t value)
{
    trapFrame->SupervisorRegisters.Epc = value;
}
