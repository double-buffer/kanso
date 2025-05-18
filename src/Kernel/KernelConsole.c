#include "Types.h"
#include "Platform.h"

// TODO: See https://github.com/riscv-software-src/opensbi/blob/master/lib/sbi/sbi_console.c#L440 for more implementation details
void KernelConsolePrint(const char* message, ...)
{
    char output[2048] = {};

    __builtin_va_list vargs;
    __builtin_va_start(vargs, message);
    
    uint32_t length = 0;

    while (*message)
    {
        if (*message == '%')
        {
            message++;

            switch (*message)
            {
                case '\0':
                {
                    message--;
                    break;
                }

                case '%':
                {
                    output[length++] = '%';
                    break;
                }

                case 's':
                {
                    const char *stringArgument = __builtin_va_arg(vargs, const char *);
                    
                    while (*stringArgument) 
                    {
                        output[length++] = *stringArgument;
                        stringArgument++;
                    }
                    break;
                }

                case 'd':
                {
                    int32_t decimalArgument = __builtin_va_arg(vargs, int32_t);

                    uint32_t magnitude = decimalArgument;

                    if (decimalArgument < 0) 
                    {
                        output[length++] = '-';
                        magnitude = -magnitude;
                    }

                    uint32_t divisor = 1;

                    while ((magnitude / divisor) > 9)
                    {
                        divisor *= 10;
                    }

                    while (divisor > 0) 
                    {
                        output[length++] = '0' + magnitude / divisor;

                        magnitude %= divisor;
                        divisor /= 10;
                    }
                    break;
                }

                case 'x':
                {
                    uint64_t hexaArgument = __builtin_va_arg(vargs, uint64_t);
                    output[length++] = '0';
                    output[length++] = 'x';

                    for (int64_t i = 15; i >= 0; i--) 
                    {
                        unsigned nibble = (hexaArgument >> (i * 4)) & 0xf;
                        output[length++] = "0123456789abcdef"[nibble];
                    }
                    break;
                }
            }
        }
        else 
        {
            output[length++] = *message;
        }

        message++;
    }

    __builtin_va_end(vargs);
    
    BiosDebugConsoleWrite(output, length);
}
