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

bool hasTestTrapHandler_WithTimerInterruptRun = false;

void TestTrapHandler_WithTimerInterrupt(CpuTrapFrame* trapFrame)
{
    hasTestTrapHandler_WithTimerInterruptRun = true;

    CpuClearPendingInterrupts(CpuInterruptType_Timer);
    CpuDisableInterrupts(CpuInterruptType_All);
    CpuSetTrapHandler(nullptr);

    auto trapCause = CpuTrapFrameGetCause(trapFrame);

    TestAssertEquals(CpuTrapCauseType_Interrupt, trapCause.Type);
    TestAssertEquals(CpuInterruptType_Timer, trapCause.InterruptType);
}

Test(Cpu, CpuTrapHandler_WithTimerInterrupt_HasCorrectCause)
{
    // Arrange
    CpuSetTrapHandler(TestTrapHandler_WithTimerInterrupt);
    CpuEnableInterrupts(CpuInterruptType_Timer);

    // Act
    BiosSetTimer(CpuReadTime());

    // Assert
    // TODO: AssertIsTrue
    TestAssertEquals(true, hasTestTrapHandler_WithTimerInterruptRun);
}

bool hasTestTrapHandler_WithInvalidInstructionRun = false;

void TestTrapHandler_WithInvalidInstruction(CpuTrapFrame* trapFrame)
{
    hasTestTrapHandler_WithInvalidInstructionRun = true;
    CpuSetTrapHandler(nullptr);

    auto nextInstructionAddress = CpuComputeNextInstructionAddress(CpuTrapFrameGetProgramCounter(trapFrame));
    CpuTrapFrameSetProgramCounter(trapFrame, nextInstructionAddress);

    // Assert
    auto trapCause = CpuTrapFrameGetCause(trapFrame);

    TestAssertEquals(CpuTrapCauseType_Interrupt, trapCause.Type);
    TestAssertEquals(CpuInterruptType_Timer, trapCause.InterruptType);
}

Test(Cpu, CpuTrapHandler_WithInvalidInstruction_HasCorrectCause)
{
    // Arrange
    CpuSetTrapHandler(TestTrapHandler_WithInvalidInstruction);

    // Act
    CpuGenerateInvalidInstruction();

    // Assert
    TestAssertEquals(true, hasTestTrapHandler_WithInvalidInstructionRun);
}
