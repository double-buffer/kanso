#pragma once

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
typedef unsigned long long uint64_t;

typedef char int8_t;
typedef short int16_t;
typedef int int32_t;
typedef long long int64_t;

// TODO: Check platform
typedef uint64_t size_t;

static_assert(sizeof(uint8_t) == 1, "uint8_t must be 1 byte.");
static_assert(sizeof(uint16_t) == 2, "uint16_t must be 2 bytes.");
static_assert(sizeof(uint32_t) == 4,  "uint32_t must be 4 bytes");
static_assert(sizeof(uint64_t) == 8,  "uint64_t must be 8 bytes");


#define va_list  __builtin_va_list
#define va_start __builtin_va_start
#define va_end   __builtin_va_end
#define va_arg   __builtin_va_arg
