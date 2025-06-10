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

void TestTrapHandler(CpuTrapFrame* trapFrame)
{
    auto programCounter = CpuTrapFrameGetProgramCounter(trapFrame);
    KernelConsolePrint(String("Test Trap %x\n"), programCounter);
    TestAssertEquals(0, programCounter);

    CpuClearPendingInterrupts(CpuInterruptType_Timer);
    CpuDisableInterrupts(CpuInterruptType_All);
}

Test(Cpu, CpuTrapHandler)
{
    // Arrange
    CpuSetTrapHandler(TestTrapHandler);
    CpuEnableInterrupts(CpuInterruptType_Timer);
    BiosSetTimer(CpuReadTime());
    KernelConsolePrint(String("Test finieshed \n"));
    CpuSetTrapHandler(nullptr);
}
