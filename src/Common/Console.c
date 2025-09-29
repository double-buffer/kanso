#include "Memory.h"
#include "Console.h"

ReadOnlySpanChar FormatBoxedMessage(MemoryArena memoryArena, ReadOnlySpanChar message)
{
    auto result = MemoryArenaPushArray(char, memoryArena, 1024);
    MemorySet(result, 0);

    auto destination = result;

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

    // TODO: There is a problem here with null terminator not present

    return ToReadOnlySpan(char, result);
}

void ConsoleSetForegroundColor(Color color)
{
    ConsolePrint(String("\x1b[38;2;%d;%d;%dm"), (int32_t)color.Red, (int32_t)color.Green, (int32_t)color.Blue);
}

void ConsoleResetStyle()
{
    ConsolePrint(String("\x1b[0m"));
    ConsoleSetForegroundColor(ConsoleColorNormal);
}

void ConsolePrintBoxMessage(ReadOnlySpanChar message)
{
    StackMemoryArena(stackMemoryArena);

    auto boxedMessage = FormatBoxedMessage(stackMemoryArena, message);
    ConsolePrint(String("\n%s\n"), boxedMessage);
}
