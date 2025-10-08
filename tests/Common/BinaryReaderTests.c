#include "BinaryReader.h"
#include "Test.h"

// TODO: Test end of data error

Test(BinaryReader, ReadUint8_AdvancesByOne)
{
    // Arrange
    const uint8_t testData[] = { 0xAB, 0xCD };
    auto reader = CreateBinaryReader(CreateReadOnlySpan(uint8_t, testData, sizeof(testData)), ByteOrder_LittleEndian);

    // Act
    auto v0 = BinaryReadUint8(&reader);
    auto v1 = BinaryReadUint8(&reader);

    // Assert
    //TestAssertEquals(MemoryError_None, MemoryGetLastError());

    TestAssertEquals(testData[0], v0);
    TestAssertEquals(testData[1], v1);
    TestAssertEquals(sizeof(testData), reader.CurrentOffset);
}

/*
}

Test(BinaryReader, ReadUint16_LittleEndian)
{
    // Arrange
    const uint8_t bytes[] = { 0x34, 0x12, 0x78, 0x56 };
    auto reader = CreateBinaryReader(rospan_from_array(bytes, sizeof(bytes)), ByteOrder_LittleEndian);

    // Act
    auto v0 = BinaryReadUint16(&reader);
    auto v1 = BinaryReadUint16(&reader);

    // Assert
    TestAssertEquals(0x1234u, v0);
    TestAssertEquals(0x5678u, v1);
    TestAssertEquals(4u, reader.CurrentOffset);
}

Test(BinaryReader, ReadUint16_BigEndian)
{
    // Arrange
    const uint8_t bytes[] = { 0x12, 0x34, 0x56, 0x78 };
    auto reader = CreateBinaryReader(rospan_from_array(bytes, sizeof(bytes)), ByteOrder_BigEndian);

    // Act
    auto v0 = BinaryReadUint16(&reader);
    auto v1 = BinaryReadUint16(&reader);

    // Assert
    TestAssertEquals(0x1234u, v0);
    TestAssertEquals(0x5678u, v1);
    TestAssertEquals(4u, reader.CurrentOffset);
}

Test(BinaryReader, ReadUint32_LittleEndian)
{
    // Arrange
    const uint8_t bytes[] = { 0x78, 0x56, 0x34, 0x12 };
    auto reader = CreateBinaryReader(rospan_from_array(bytes, sizeof(bytes)), ByteOrder_LittleEndian);

    // Act
    auto v = BinaryReadUint32(&reader);

    // Assert
    TestAssertEquals(0x12345678u, v);
    TestAssertEquals(4u, reader.CurrentOffset);
}

Test(BinaryReader, ReadUint32_BigEndian)
{
    // Arrange
    const uint8_t bytes[] = { 0x12, 0x34, 0x56, 0x78 };
    auto reader = CreateBinaryReader(rospan_from_array(bytes, sizeof(bytes)), ByteOrder_BigEndian);

    // Act
    auto v = BinaryReadUint32(&reader);

    // Assert
    TestAssertEquals(0x12345678u, v);
    TestAssertEquals(4u, reader.CurrentOffset);
}

Test(BinaryReader, ReadUint64_LittleEndian)
{
    // Arrange
    const uint8_t bytes[] = { 0x15, 0x81, 0xE9, 0x7D, 0xF4, 0x10, 0x22, 0x11 };
    auto reader = CreateBinaryReader(rospan_from_array(bytes, sizeof(bytes)), ByteOrder_LittleEndian);

    // Act
    auto v = BinaryReadUint64(&reader);

    // Assert
    TestAssertEquals((uint64_t)0x112210F47DE98115ULL, v);
    TestAssertEquals(8u, reader.CurrentOffset);
}

Test(BinaryReader, ReadUint64_BigEndian)
{
    // Arrange
    const uint8_t bytes[] = { 0x11, 0x22, 0x10, 0xF4, 0x7D, 0xE9, 0x81, 0x15 };
    auto reader = CreateBinaryReader(rospan_from_array(bytes, sizeof(bytes)), ByteOrder_BigEndian);

    // Act
    auto v = BinaryReadUint64(&reader);

    // Assert
    TestAssertEquals((uint64_t)0x112210F47DE98115ULL, v);
    TestAssertEquals(8u, reader.CurrentOffset);
}

Test(BinaryReader, SetOffset_ThenRead32_Works)
{
    // Arrange
    const uint8_t bytes[] = { 0,0, 0x78,0x56,0x34,0x12, 0,0 };
    auto reader = CreateBinaryReader(rospan_from_array(bytes, sizeof(bytes)), ByteOrder_LittleEndian);

    // Act
    BinarySetOffset(&reader, 2);
    auto v = BinaryReadUint32(&reader);

    // Assert
    TestAssertEquals(0x12345678u, v);
    TestAssertEquals(6u, reader.CurrentOffset); // 2 + 4
}

Test(BinaryReader, ReadCString_CopiesUntilNullAndAdvances)
{
    // Arrange
    const uint8_t bytes[] = { 'o','s','d','e','v','\0', 'x','y' };
    auto reader = CreateBinaryReader(rospan_from_array(bytes, sizeof(bytes)), ByteOrder_LittleEndian);

    auto outSpan = StackAlloc(uint8_t, 16);
    outSpan.Length = 0; // will be filled by reader

    // Act
    BinaryReadString(&reader, &outSpan);

    // Assert
    TestAssertEquals(5u, outSpan.Length);
    TestAssertEquals('o', SpanAt(outSpan, 0));
    TestAssertEquals('s', SpanAt(outSpan, 1));
    TestAssertEquals('d', SpanAt(outSpan, 2));
    TestAssertEquals('e', SpanAt(outSpan, 3));
    TestAssertEquals('v', SpanAt(outSpan, 4));
    TestAssertEquals(6u, reader.CurrentOffset); // "osdev" + '\0'
}

Test(BinaryReader, Bounds_ReadBeyondBuffer_SetsErrorAndReturnsZero)
{
    // Arrange
    const uint8_t bytes[] = { 0x01, 0x02, 0x03 }; // only 3 bytes
    auto reader = CreateBinaryReader(rospan_from_array(bytes, sizeof(bytes)), ByteOrder_LittleEndian);

    // Act
    auto v = BinaryReadUint32(&reader); // tries to read 4 bytes

    // Assert
    TestAssertEquals(0u, v);
    TestAssertEquals(TypeError_InvalidParameter, TypeGetLastError());
}

Test(BinaryReader, Bounds_StringWithoutTerminator_SetsErrorAndDoesNotOverflow)
{
    // Arrange: buffer without '\\0' before end
    const uint8_t bytes[] = { 'b','a','d' };
    auto reader = CreateBinaryReader(rospan_from_array(bytes, sizeof(bytes)), ByteOrder_LittleEndian);

    auto outSpan = StackAlloc(uint8_t, 8);
    outSpan.Length = 0;

    // Act
    BinaryReadString(&reader, &outSpan);

    // Assert: expect error and offset unchanged
    TestAssertEquals(TypeError_InvalidParameter, TypeGetLastError());
    TestAssertEquals(0u, reader.CurrentOffset);
}
*/
