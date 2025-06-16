#include "String.h"

ReadOnlySpanChar String(const char* string)
{
    return CreateReadOnlySpanChar(string, __builtin_strlen(string));
}

bool StringEquals(ReadOnlySpanChar string1, ReadOnlySpanChar string2)
{
    if (string1.Length != string2.Length)
    {
        return false;
    }

    for (uint32_t i = 0; i < string1.Length; i++)
    {
        if (string1.Pointer[i] != string2.Pointer[i])
        {
            return false;
        }
    }

    return true;
}

void StringFormat(SpanChar* destination, ReadOnlySpanChar message, ...)
{
    va_list vargs;
    va_start(vargs, message);
    StringFormatVargs(destination, message, vargs);
    va_end(vargs);
}

// TODO: Refactor this function
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
                    destination->Pointer[length++] = '%';
                    break;
                }

                case 's':
                {
                    const char* stringArgument = va_arg(vargs, const char *);
                    
                    while (*stringArgument) 
                    {
                        destination->Pointer[length++] = *stringArgument;
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
                        destination->Pointer[length++] = '-';
                        magnitude = -magnitude;
                    }

                    uint32_t divisor = 1;

                    while ((magnitude / divisor) > 9)
                    {
                        divisor *= 10;
                    }

                    while (divisor > 0) 
                    {
                        destination->Pointer[length++] = '0' + magnitude / divisor;

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
                        destination->Pointer[length++] = '-';
                        magnitude = -magnitude;
                    }

                    int32_t divisor = 1;

                    while ((magnitude / divisor) > 9)
                    {
                        divisor *= 10;
                    }

                    while (divisor > 0) 
                    {
                        destination->Pointer[length++] = '0' + magnitude / divisor;

                        magnitude %= divisor;
                        divisor /= 10;
                    }
                    break;
                }

                case 'x':
                {
                    uintptr_t hexaArgument = va_arg(vargs, uintptr_t);
                    destination->Pointer[length++] = '0';
                    destination->Pointer[length++] = 'x';

                    for (int32_t i = (sizeof(uintptr_t) * 2) - 1; i >= 0; i--) 
                    {
                        unsigned nibble = (hexaArgument >> (i * 4)) & 0xf;
                        destination->Pointer[length++] = "0123456789abcdef"[nibble];
                    }
                    break;
                }
            }
        }
        else 
        {
            destination->Pointer[length++] = *messagePointer;
        }

        messagePointer++;
    }

    destination->Length = length;
    destination->Pointer[length] = '\0';
}

