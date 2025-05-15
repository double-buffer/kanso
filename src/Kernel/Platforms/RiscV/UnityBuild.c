typedef unsigned char uint8_t;
typedef int int32_t;
typedef unsigned int uint32_t;
typedef unsigned long uint64_t;
typedef long int64_t;
typedef uint64_t size_t;

extern char __bss[], __bss_end[];

void *memset(void *buf, char c, size_t n) 
{
    uint8_t *p = (uint8_t *) buf;
    while (n--)
        *p++ = c;
    return buf;
}

typedef enum
{
    CpuInterruptType_None = 0,
    CpuInterruptType_Software = 1,
    CpuInterruptType_Timer = 2,
    CpuInterruptType_External = 4,
} CpuInterruptType;

static inline uint64_t ComputeCpuInterruptMask(CpuInterruptType types)
{
    uint64_t mask = 0ul;

    if (types & CpuInterruptType_Software)
    {
        mask |= (1ul << 1);
    }

    if (types & CpuInterruptType_Timer)
    {
        mask |= (1ul << 5);
    }

    if (types & CpuInterruptType_External)
    {
        mask |= (1ul << 9);
    }

    return mask;
}

__attribute__((always_inline))
static inline uint64_t CpuReadTime()
{
    uint64_t result;
    __asm__ volatile("rdtime %0" : "=r" (result));
    return result;
}

__attribute__((always_inline))
static inline uint64_t CpuReadCycle()
{
    uint64_t result;
    __asm__ volatile("rdcycle %0" : "=r" (result));
    return result;
}

__attribute__((always_inline))
static inline void CpuSetSupervisorTrapHandler(void* trapHandler)
{
    __asm__ volatile(
      "csrw stvec, %0\n"
      "csrsi sstatus, 2"
      : 
      : "r" (trapHandler));
}

__attribute__((always_inline))
static inline void CpuEnableSupervisorInterrupts(CpuInterruptType types)
{
    uint64_t mask = ComputeCpuInterruptMask(types);
    __asm__ volatile (
        "csrs sie, %0"
        :
        : "r"(mask)
        : "memory"
    );
}

__attribute__((always_inline))
static inline void CpuDisableSupervisorInterrupts(CpuInterruptType types)
{
    uint64_t mask = ComputeCpuInterruptMask(types);

    __asm__ volatile (
        "csrc sie, %0"
        :
        : "r"(mask)
        : "memory"
    );
}

__attribute__((always_inline))
static inline void CpuClearSupervisorPendingInterrupts(CpuInterruptType types)
{
    uint64_t mask = ComputeCpuInterruptMask(types);

    __asm__ volatile (
        "csrc sip, %0"
        :
        : "r"(mask)
        : "memory"
    );
}

__attribute__((always_inline))
static inline void CpuWaitForInterrupt()
{
    __asm__ __volatile__("wfi");
}

// Doc on SBI is available here: https://github.com/riscv-non-isa/riscv-sbi-doc/tree/master

typedef enum : int64_t
{
    SbiReturnCode_Success = 0,
    SbiReturnCode_ErrorFailed = -1,
    SbiReturnCode_ErrorNotSupported = -2,
    SbiReturnCode_ErrorInvalidParameter = -3,
    SbiReturnCode_ErrorDenied = -4,
    SbiReturnCode_ErrorInvalidAddress = -5,
    SbiReturnCode_ErrorAlreadyAvailable = -6,
    SbiReturnCode_ErrorAlreadyStarted = -7,
    SbiReturnCode_ErrorAlreadyStopped = -8,
    SbiReturnCode_ErrorNoSharedMemory = -9,
    SbiReturnCode_ErrorInvalidState = -10,
    SbiReturnCode_ErrorBadRange = -11,
    SbiReturnCode_ErrorTimeout = -12,
    SbiReturnCode_ErrorIO = -13,
    SbiReturnCode_ErrorDeniedLocked = -14
} SbiReturnCode;

typedef enum
{
    SbiExtension_DebugConsole = 0x4442434E,
    SbiExtension_Time = 0x54494D45
} SbiExtension;

typedef struct
{
    SbiReturnCode ReturnCode;
    int64_t Value;
} SbiReturn;

SbiReturn SbiCallFunction(uint64_t extensionId, uint64_t functionId, 
                     uint64_t parameter0, uint64_t parameter1, uint64_t parameter2,
                     uint64_t parameter3, uint64_t parameter4, uint64_t parameter5)
{
    register uint64_t a0 asm ("a0") = parameter0;
    register uint64_t a1 asm ("a1") = parameter1;
    register uint64_t a2 asm ("a2") = parameter2;
    register uint64_t a3 asm ("a3") = parameter3;
    register uint64_t a4 asm ("a4") = parameter4;
    register uint64_t a5 asm ("a5") = parameter5;
    register uint64_t a6 asm ("a6") = functionId;
    register uint64_t a7 asm ("a7") = extensionId;

    __asm__ __volatile__(
        "ecall" 
        : "+r" (a0), "+r" (a1)
        : "r" (a2), "r" (a3), "r" (a4), "r" (a5), "r" (a6), "r" (a7)
        : "memory");

    return (SbiReturn)
    {
        .ReturnCode = a0,
        .Value = a1
    };
}

void SbiDebugConsoleWrite(const char* message, uint32_t length)
{
    SbiCallFunction(SbiExtension_DebugConsole, 0x00, length, (uint64_t)message, 0, 0, 0, 0);
}

void SbiSetTimer(uint64_t timeValue)
{
    SbiCallFunction(SbiExtension_Time, 0x00, timeValue, 0, 0, 0, 0, 0);
}

// TODO: See https://github.com/riscv-software-src/opensbi/blob/master/lib/sbi/sbi_console.c#L440 for more implementation details
void KernelConsolePrint(const char* message, ...)
{
    char output[2048];
    memset(output, 0, 2048);

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
    
    SbiDebugConsoleWrite(output, length);
}

__attribute__((interrupt("supervisor")))
__attribute__((section(".text.interrupt")))
void KernelSupervisorTrapHandler()
{
    CpuClearSupervisorPendingInterrupts(CpuInterruptType_Timer);
    KernelConsolePrint("Kernel trap handler: %d.\n", CpuReadTime());

    //CpuDisableSupervisorInterrupts(CpuInterruptType_Timer);
    //SbiSetTimer((uint64_t)-1);
    SbiSetTimer(CpuReadTime() + 10000000);
}

void KernelMain()
{
    // TODO: Can we move this initialisation to the assembly code?
    memset(__bss, 0, (size_t) __bss_end - (size_t) __bss);

    KernelConsolePrint("Kanso OS 1.0-DEV1\n");

    CpuSetSupervisorTrapHandler(KernelSupervisorTrapHandler);
    SbiSetTimer(CpuReadTime() + 10000000);
    CpuEnableSupervisorInterrupts(CpuInterruptType_Timer);

    while(1)
    {
        CpuWaitForInterrupt();
    }   
}
