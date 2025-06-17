#pragma once

#include "Memory.h"
#include "Types.h"

// --------------------------------------------------------------------------------------
// General
// --------------------------------------------------------------------------------------

typedef struct
{
    ReadOnlySpanChar Name;
    uint32_t ArchitectureBits;
    uint8_t BootCpuId;
    // TODO: Add Board Name / Computer name / Platform form name
} PlatformInformation;

// TODO: We should maybe put that in common and call it SystemDevice or SystemStaticDevice
typedef struct
{
} PlatformDevices;

PlatformInformation PlatformGetInformation();
PlatformDevices PlatformGetDevices();

// --------------------------------------------------------------------------------------
// Cpu
// --------------------------------------------------------------------------------------

typedef enum
{
    CpuInterruptType_None = 0x00,
    CpuInterruptType_Software = 0x01,
    CpuInterruptType_Timer = 0x02,
    CpuInterruptType_External = 0x04,
    CpuInterruptType_All = 0xFF,
} CpuInterruptType;

typedef enum 
{
    CpuTrapType_Unknown,
    CpuTrapType_Interrupt,
    CpuTrapType_Synchronous
} CpuTrapType;

typedef enum
{
    CpuTrapSynchronousType_Unknown,
    CpuTrapSynchronousType_InstructionError,
    CpuTrapSynchronousType_Debug,
    CpuTrapSynchronousType_AddressError,
    CpuTrapSynchronousType_PageError,
    CpuTrapSynchronousType_SystemCall,
    CpuTrapSynchronousType_IntegrityError,
    CpuTrapSynchronousType_HardwareError
} CpuTrapSynchronousType;

typedef struct
{
    CpuTrapType Type;
    CpuInterruptType InterruptType;
    CpuTrapSynchronousType SynchronousType;
    uintptr_t Code;
    uintptr_t ExtraInformation;
} CpuTrapCause;

struct CpuTrapFrame;
typedef struct CpuTrapFrame CpuTrapFrame;

typedef void (*CpuTrapHandler)(struct CpuTrapFrame*);

uint64_t CpuReadTime();
uint64_t CpuReadCycle();

void CpuGenerateInvalidInstruction();
uintptr_t CpuComputeNextInstructionAddress(uintptr_t instructionAddress);

void CpuSetTrapHandler(CpuTrapHandler trapHandler);
void CpuEnableInterrupts(CpuInterruptType types);
void CpuDisableInterrupts(CpuInterruptType types);
void CpuClearPendingInterrupts(CpuInterruptType types);
void CpuWaitForInterrupt();

void CpuLogTrapFrame(const CpuTrapFrame* trapFrame);
CpuTrapCause CpuTrapFrameGetCause(const CpuTrapFrame* trapFrame);
uintptr_t CpuTrapFrameGetProgramCounter(const CpuTrapFrame* trapFrame);
void CpuTrapFrameSetProgramCounter(CpuTrapFrame* trapFrame, uintptr_t value);


// --------------------------------------------------------------------------------------
// Bios
// --------------------------------------------------------------------------------------

typedef enum
{
    BiosResetType_Shutdown = 0,
    BiosResetType_ColdReboot = 1,
    BiosResetType_WarmReboot = 2,
} BiosResetType;

typedef enum
{
    BiosResetReason_None = 0,
    BiosResetReason_SystemFailure = 1,
} BiosResetReason;

void BiosDebugConsoleWrite(ReadOnlySpanChar message);
void BiosSetTimer(uint64_t timeValue);
void BiosReset(BiosResetType resetType, BiosResetReason reason);
