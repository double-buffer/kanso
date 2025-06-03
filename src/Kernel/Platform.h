#pragma once

#include "Memory.h"
#include "Types.h"

typedef struct
{
    ReadOnlySpanChar Name;
    uint32_t ArchitectureBits;
} PlatformInformation;

PlatformInformation PlatformGetInformation();

typedef enum
{
    CpuInterruptType_None = 0,
    CpuInterruptType_Software = 1,
    CpuInterruptType_Timer = 2,
    CpuInterruptType_External = 4,
    CpuInterruptType_All = 0xFF,
} CpuInterruptType;

struct CpuTrapFrame;
typedef struct CpuTrapFrame CpuTrapFrame;

typedef void (*CpuTrapHandler)(struct CpuTrapFrame*);

uint64_t CpuReadTime();
uint64_t CpuReadCycle();
void CpuSetSupervisorTrapHandler(CpuTrapHandler trapHandler);
void CpuEnableSupervisorInterrupts(CpuInterruptType types);
void CpuDisableSupervisorInterrupts(CpuInterruptType types);
void CpuClearSupervisorPendingInterrupts(CpuInterruptType types);
void CpuWaitForInterrupt();

uintptr_t CpuTrapFrameGetProgramCounter(const CpuTrapFrame* trapFrame);


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
