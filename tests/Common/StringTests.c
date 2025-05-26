#include "String.h"
#include "Test.h"

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
    const auto testString = String("Test: %d, %s");
    const auto intParameter = 28;
    const auto stringParameter = String("TestParameter");
    
    const auto finalString = String("Test: 28, TestParameter"); 
    auto destination = StackAllocChar(64);

    // Act
    StringFormat(&destination, testString, intParameter, stringParameter);

    // Assert
    TestAssertStringEquals(finalString, ToReadOnlySpanChar(destination));
}

Test(String, StringEquals_WithSameStrings_ReturnsTrue)
{
    // Arrange
    const auto testString1 = String("Test String");
    const auto testString2 = String("Test String");

    // Act
    auto result = StringEquals(testString1, testString2);

    // Assert
    TestAssertEquals(true, result);
}

Test(String, StringEquals_WithDifferentStrings_ReturnsFalse)
{
    // Arrange
    const auto testString1 = String("Test String");
    const auto testString2 = String("Bad String");

    // Act
    auto result = StringEquals(testString1, testString2);

    // Assert
    TestAssertEquals(false, result);
}
