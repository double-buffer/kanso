#include "Test.h"
#include "Memory.h"

Test(String, String_HasCorrectValues)
{
    // Arrange
    const char* testString = "HelloTest";
    const uint32_t testStringLength = 9;
    
    // Act
    auto result = String(testString);

    // Assert
    TestAssertEquals(testStringLength, result.Length);

    for (uint32_t i = 0; i < result.Length; i++)
    {
        TestAssertEquals(testString[i], result.Pointer[i]);
    }
}

Test(String, StringFormat_WithParameters_HasCorrectValues)
{
    // Arrange
    const auto testString = String("Test: %d, %s\n");
    const auto intParameter = 28;
    const auto stringParameter = String("TestParameter");
    
    const auto finalString = String("Test: 28, TestParameter\n"); 
    auto destination = StackAllocChar(64);

    char testData[255];
    //auto destination = Span(testData, 255);

    // Act
    // TODO: Remove number 2
    StringFormat2(&destination, testString, intParameter, stringParameter);

    // Assert
    // TODO: TestAssertString
    TestAssertEquals(finalString.Length, destination.Length);

    for (uint32_t i = 0; i < destination.Length; i++)
    {
        TestAssertEquals(finalString.Pointer[i], destination.Pointer[i]);
    }
}

