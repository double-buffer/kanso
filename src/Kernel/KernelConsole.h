#pragma once

#include "Memory.h"
#include "String.h"
#include "Types.h"

const Color KernelConsoleColorNormal = { 212, 212, 212, 255 };
const Color KernelConsoleColorHighlight = { 250, 250, 250, 255 };
const Color KernelConsoleColorAccent = { 79, 193, 255, 255 };
const Color KernelConsoleColorSuccess = { 106, 153, 85, 255 };
const Color KernelConsoleColorWarning = { 255, 135, 100, 255 };
const Color KernelConsoleColorError = { 255, 105, 105, 255 };
const Color KernelConsoleColorInfo = { 220, 220, 170, 255 };
const Color KernelConsoleColorAction = { 197, 134, 192, 255 };
const Color KernelConsoleColorKeyword = { 86, 156, 214, 255 };
const Color KernelConsoleColorNumeric = { 181, 206, 168, 255 };

void KernelConsolePrint(ReadOnlySpanChar message, ...);

void KernelConsoleSetForegroundColor(Color color);
void KernelConsoleResetStyle();

void KernelConsolePrintBoxMessage(ReadOnlySpanChar message);
