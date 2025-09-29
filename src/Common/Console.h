#pragma once

#include "Memory.h"
#include "String.h"
#include "Types.h"

const Color ConsoleColorNormal = { 212, 212, 212, 255 };
const Color ConsoleColorHighlight = { 250, 250, 250, 255 };
const Color ConsoleColorAccent = { 79, 193, 255, 255 };
const Color ConsoleColorSuccess = { 106, 153, 85, 255 };
const Color ConsoleColorWarning = { 255, 135, 100, 255 };
const Color ConsoleColorError = { 255, 105, 105, 255 };
const Color ConsoleColorInfo = { 220, 220, 170, 255 };
const Color ConsoleColorAction = { 197, 134, 192, 255 };
const Color ConsoleColorKeyword = { 86, 156, 214, 255 };
const Color ConsoleColorNumeric = { 181, 206, 168, 255 };

void ConsolePrint(ReadOnlySpanChar message, ...);

void ConsoleSetForegroundColor(Color color);
void ConsoleResetStyle();

void ConsolePrintBoxMessage(ReadOnlySpanChar message);
