#include "AsmCommon.h"

.global supervisor_trap_entry

.section .text.interrupt

supervisor_trap_entry:
    sret
