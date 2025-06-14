#include "KernelConsole.h"
#include "Memory.h"
#include "String.h"
#include "Platform.h"

void KernelConsolePrint(ReadOnlySpanChar message, ...)
{
    auto output = StackAllocChar(2048);

    va_list vargs;
    va_start(vargs, message);
    
    StringFormatVargs(&output, message, vargs);

    va_end(vargs);
    
    BiosDebugConsoleWrite(ToReadOnlySpanChar(output));
}

void KernelConsoleSetForegroundColor(Color color)
{
    KernelConsolePrint(String("\x1b[38;2;%d;%d;%dm"), (int32_t)color.Red, (int32_t)color.Green, (int32_t)color.Blue);
}

void KernelConsoleResetStyle()
{
    KernelConsolePrint(String("\x1b[0m"));
    KernelConsoleSetForegroundColor(KernelConsoleColorNormal);
}

void FormatBoxedMessage(SpanChar destination, ReadOnlySpanChar message)
{
    auto upLeftCorner = String("┌");
    auto upRightCorner = String("┐");
    auto downLeftCorner = String("└");
    auto downRightCorner = String("┘");
    auto horizontalLine = String("─");
    auto verticalLine = String("│");

    MemoryCopy(destination, upLeftCorner);
    destination = SpanSliceFrom(destination, upLeftCorner.Length);

    for (uint32_t i = 0; i < message.Length + 2; i++)
    {
        MemoryCopy(destination, horizontalLine);
        destination = SpanSliceFrom(destination, horizontalLine.Length);
    }

    MemoryCopy(destination, upRightCorner);
    destination = SpanSliceFrom(destination, upRightCorner.Length);

    MemoryCopy(destination, String("\n"));
    destination = SpanSliceFrom(destination, 1);

    MemoryCopy(destination, verticalLine);
    destination = SpanSliceFrom(destination, verticalLine.Length);

    MemoryCopy(destination, String(" "));
    destination = SpanSliceFrom(destination, 1);

    MemoryCopy(destination, message);
    destination = SpanSliceFrom(destination, message.Length);

    MemoryCopy(destination, String(" "));
    destination = SpanSliceFrom(destination, 1);

    MemoryCopy(destination, verticalLine);
    destination = SpanSliceFrom(destination, verticalLine.Length);

    MemoryCopy(destination, String("\n"));
    destination = SpanSliceFrom(destination, 1);

    MemoryCopy(destination, downLeftCorner);
    destination = SpanSliceFrom(destination, downLeftCorner.Length);

    for (uint32_t i = 0; i < message.Length + 2; i++)
    {
        MemoryCopy(destination, horizontalLine);
        destination = SpanSliceFrom(destination, horizontalLine.Length);
    }

    MemoryCopy(destination, downRightCorner);
    destination = SpanSliceFrom(destination, downRightCorner.Length);
}

void KernelConsolePrintBoxMessage(ReadOnlySpanChar message)
{
    auto boxedMessage = StackAllocChar(512);
    FormatBoxedMessage(boxedMessage, message);
    KernelConsolePrint(String("\n%s\n"), boxedMessage);
}
