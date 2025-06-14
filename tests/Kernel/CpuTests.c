#include "Test.h"
#include "../Kernel/Platform.h"

// TODO: Supervisor trap handler: Add test that test if the registers are well saved and restored!
// TODO: Test each trap frame get/set functions

Test(Cpu, CpuReadTime)
{
    // Arrange
    uint64_t time1 = 0;
    uint64_t time2 = 0;
    
    // Act
    time1 = CpuReadTime();
    time2 = CpuReadTime();

    // Assert
    TestAssertNotEquals(0, time1);
    TestAssertNotEquals(0, time2);
    TestAssertGreaterThan(time2, time1);
}

Test(Cpu, CpuReadCycle)
{
    // Arrange
    uint64_t cycle1 = 0;
    uint64_t cycle2 = 0;
    
    // Act
    cycle1 = CpuReadCycle();
    cycle2 = CpuReadCycle();

    // Assert
    TestAssertNotEquals(0, cycle1);
    TestAssertNotEquals(0, cycle2);
    TestAssertGreaterThan(cycle2, cycle1);
}

volatile bool hasTestTrapHandler_WithTimerInterruptRun = false;

void TestTrapHandler_WithTimerInterrupt(CpuTrapFrame* trapFrame)
{
    hasTestTrapHandler_WithTimerInterruptRun = true;

    CpuClearPendingInterrupts(CpuInterruptType_Timer);
    CpuDisableInterrupts(CpuInterruptType_All);
    CpuSetTrapHandler(nullptr);

    auto trapCause = CpuTrapFrameGetCause(trapFrame);

    TestAssertEquals(CpuTrapType_Interrupt, trapCause.Type);
    TestAssertEquals(CpuInterruptType_Timer, trapCause.InterruptType);
}

Test(Cpu, CpuTrapHandler_WithTimerInterrupt_HasCorrectCause)
{
    // Arrange
    CpuSetTrapHandler(TestTrapHandler_WithTimerInterrupt);
    CpuEnableInterrupts(CpuInterruptType_Timer);
    auto timerDeadline = CpuReadTime();

    // Act
    BiosSetTimer(timerDeadline);

    // Assert
    const uint32_t maxIterations = 10;
    uint32_t iterations = 0;

    while (!hasTestTrapHandler_WithTimerInterruptRun && iterations < maxIterations) 
    {
        iterations++;
    }

    TestAssertIsTrue(hasTestTrapHandler_WithTimerInterruptRun);
}

bool hasTestTrapHandler_WithInvalidInstructionRun = false;

void TestTrapHandler_WithInvalidInstruction(CpuTrapFrame* trapFrame)
{
    hasTestTrapHandler_WithInvalidInstructionRun = true;

    auto trapCause = CpuTrapFrameGetCause(trapFrame);

    TestAssertEquals(CpuTrapType_Synchronous, trapCause.Type);
    TestAssertEquals(CpuTrapSynchronousType_InstructionError, trapCause.SynchronousType);

    auto programCounter = CpuTrapFrameGetProgramCounter(trapFrame);
    auto nextInstructionAddress = CpuComputeNextInstructionAddress(programCounter);
    CpuTrapFrameSetProgramCounter(trapFrame, nextInstructionAddress);
}

Test(Cpu, CpuTrapHandler_WithInvalidInstruction_HasCorrectCause)
{
    // Arrange
    CpuSetTrapHandler(TestTrapHandler_WithInvalidInstruction);

    // Act
    CpuGenerateInvalidInstruction();

    // Assert
    TestAssertEquals(true, hasTestTrapHandler_WithInvalidInstructionRun);

    CpuSetTrapHandler(nullptr);
}
