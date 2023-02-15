#include "regs-save-restore.h"

@MK_FN(switchto_user_asm)
@    mov sp, r0
@    @ load user copies.
@    ldm   sp, {r0-r14}^
@    @ spsr and pc.
@    add  sp, sp, #60
@    rfe sp          @ rfe loads [pc and cpsr]
@    asm_not_reached()


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


