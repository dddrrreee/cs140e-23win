#include "regs-save-restore.h"

MK_FN(switchto_user_asm)
    asm_todo("implement this");

@ save all the registers in ascending order.
swi_trampoline:
    TRAMPOLINE_FULL_REGS(do_syscall)

@ only handle single step.
.align 5; 
.globl test_restore_handlers
test_restore_handlers:
    b unhandled_reset
    b unhandled_undefined_instruction
    b swi_trampoline
    b unhandled_prefetch_abort 
    b unhandled_data_abort
    b unhandled_interrupt
    b unhandled_fiq


