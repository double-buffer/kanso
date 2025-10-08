#pragma once

#include "Types.h"

// TODO: Write tests
// TODO: Maybe we should group the code so that for the error handling we have a kind of module

typedef struct
{
    ReadOnlySpanUint8 Data;
    ByteOrder ByteOrder; 
    size_t CurrentOffset;
} BinaryReader;

BinaryReader CreateBinaryReader(ReadOnlySpanUint8 data, ByteOrder byteOrder);
void BinarySetOffset(BinaryReader* reader, size_t offset);

uint8_t BinaryReadUint8(BinaryReader* reader);
uint32_t BinaryReadUint32(BinaryReader* reader);
uint64_t BinaryReadUint64(BinaryReader* reader);

// TODO: When we have memoryarena we can maybe do better
void BinaryReadBytes(BinaryReader* reader, size_t length, SpanUint8* output);

// TODO: When we have memoryarena we can maybe do better
void BinaryReadString(BinaryReader* reader, SpanChar* output);
