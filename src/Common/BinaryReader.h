#pragma once

#include "Types.h"

// TODO: Write tests

typedef struct
{
    ReadOnlySpanUint8 Data;
    ByteOrder ByteOrder; 
    size_t CurrentOffset;
} BinaryReader;

BinaryReader CreateBinaryReader(ReadOnlySpanUint8 data, ByteOrder byteOrder);
void BinarySetOffset(BinaryReader* reader, size_t offset);

uint32_t BinaryReadUint32(BinaryReader* reader);
uint64_t BinaryReadUint64(BinaryReader* reader);

// TODO: When we have memoryarena we can maybe do better
void BinaryReadBytes(BinaryReader* reader, size_t length, SpanUint8* output);

// TODO: When we have memoryarena we can maybe do better
void BinaryReadString(BinaryReader* reader, SpanChar* output);
