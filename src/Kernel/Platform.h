#pragma once

#include "Types.h"

typedef enum
{
    CpuInterruptType_None = 0,
    CpuInterruptType_Software = 1,
    CpuInterruptType_Timer = 2,
    CpuInterruptType_External = 4,
} CpuInterruptType;

typedef void (*CpuTrapHandler)();

uint64_t CpuReadTime();
uint64_t CpuReadCycle();
void CpuSetSupervisorTrapHandler(CpuTrapHandler trapHandler);
void CpuEnableSupervisorInterrupts(CpuInterruptType types);
void CpuDisableSupervisorInterrupts(CpuInterruptType types);
void CpuClearSupervisorPendingInterrupts(CpuInterruptType types);
void CpuWaitForInterrupt();


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

void BiosDebugConsoleWrite(const char* message, uint32_t length);
void BiosSetTimer(uint64_t timeValue);
void BiosReset(BiosResetType resetType, BiosResetReason reason);
