#include "regs-save-restore.h"

@ save all the registers in ascending order.
swi_trampoline:
    mov sp, #INT_STACK_ADDR
    TRAMPOLINE_FULL_REGS(do_syscall)

single_step_trampoline:
    mov sp, #INT_STACK_ADDR
    sub   lr, lr, #4
    TRAMPOLINE_FULL_REGS(single_step_full)

@ only handle single step.
.align 5; 
.globl test5_full_single
test5_full_single:
    b unhandled_reset
    b unhandled_undefined_instruction
    b swi_trampoline
    b single_step_trampoline
    b unhandled_data_abort
    b unhandled_interrupt
    b unhandled_fiq


