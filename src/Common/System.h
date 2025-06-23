#pragma once

#include "Memory.h"

typedef struct
{
    ReadOnlySpanChar Name;
    uint32_t ArchitectureBits;
    uint32_t PageSize;
    // TODO: Add Board Name / Computer name / Platform form name
} SystemInformation;

SystemInformation SystemGetInformation();
