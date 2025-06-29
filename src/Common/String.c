#include "String.h"
#include "Memory.h"

bool StringEquals(ReadOnlySpanChar string1, ReadOnlySpanChar string2)
{
    if (string1.Length != string2.Length)
    {
        return false;
    }

    for (uint32_t i = 0; i < string1.Length; i++)
    {
        if (SpanAt(string1, i) != SpanAt(string2, i))
        {
            return false;
        }
    }

    return true;
}

// TODO: Replace that with a memory arena
void StringSplit(SpanString* result, ReadOnlySpanChar value, char separator)
{
    auto resultCount = 0;
    auto currentStartIndex = 0;

    for (uint32_t i = 0; i < value.Length; i++)
    {
        if (SpanAt(value, i) == separator)
        {
            SpanAt(*result, resultCount++) = SpanSlice(value, currentStartIndex, i - currentStartIndex);
            currentStartIndex = i + 1;
        }
    }

    SpanAt(*result, resultCount++) = SpanSlice(value, currentStartIndex, value.Length - currentStartIndex);

    result->Length = resultCount;
}

// TODO: Replace that with a memory arena
void StringFormat(SpanChar* destination, ReadOnlySpanChar message, ...)
{
    va_list vargs;
    va_start(vargs, message);
    StringFormatVargs(destination, message, vargs);
    va_end(vargs);
}

// TODO: Refactor this function
// TODO: It would be cool if we could handle ReadOnlySpan so we can take the length not until \0
void StringFormatVargs(SpanChar* destination, ReadOnlySpanChar message, va_list vargs)
{
    uint32_t length = 0;
    char* messagePointer = (char*)message.Pointer;

    while (*messagePointer)
    {
        if (*messagePointer == '%')
        {
            messagePointer++;

            switch (*messagePointer)
            {
                case '\0':
                {
                    messagePointer--;
                    break;
                }

                case '%':
                {
                    SpanAt(*destination, length++) = '%';
                    break;
                }

                case 's':
                {
                    const char* stringArgument = va_arg(vargs, const char *);
                    
                    while (*stringArgument) 
                    {
                        SpanAt(*destination, length++) = *stringArgument;
                        stringArgument++;
                    }
                    break;
                }

                case 'd':
                {
                    int32_t decimalArgument = va_arg(vargs, int32_t);

                    uint32_t magnitude = decimalArgument;

                    if (decimalArgument < 0) 
                    {
                        SpanAt(*destination, length++) = '-';
                        magnitude = -magnitude;
                    }

                    uint32_t divisor = 1;

                    while ((magnitude / divisor) > 9)
                    {
                        divisor *= 10;
                    }

                    while (divisor > 0) 
                    {
                        SpanAt(*destination, length++) = '0' + magnitude / divisor;

                        magnitude %= divisor;
                        divisor /= 10;
                    }
                    break;
                }

                case 'l':
                {
                    int64_t decimalArgument = va_arg(vargs, int64_t);

                    // HACK: We should do a code that compile 32 and 64-bit
                    int32_t magnitude = (int32_t)decimalArgument;

                    if (decimalArgument < 0) 
                    {
                        SpanAt(*destination, length++) = '-';
                        magnitude = -magnitude;
                    }

                    int32_t divisor = 1;

                    while ((magnitude / divisor) > 9)
                    {
                        divisor *= 10;
                    }

                    while (divisor > 0) 
                    {
                        SpanAt(*destination, length++) = '0' + magnitude / divisor;

                        magnitude %= divisor;
                        divisor /= 10;
                    }
                    break;
                }

                case 'x':
                {
                    uintptr_t hexaArgument = va_arg(vargs, uintptr_t);
                    SpanAt(*destination, length++) = '0';
                    SpanAt(*destination, length++) = 'x';

                    for (int32_t i = (sizeof(uintptr_t) * 2) - 1; i >= 0; i--) 
                    {
                        unsigned nibble = (hexaArgument >> (i * 4)) & 0xf;
                        SpanAt(*destination, length++) = "0123456789abcdef"[nibble];
                    }
                    break;
                }
            }
        }
        else 
        {
            SpanAt(*destination, length++) = *messagePointer;
        }

        messagePointer++;
    }

    destination->Length = length;
    SpanAt(*destination, length) = '\0';
}
