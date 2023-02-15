#include "regs-save-restore.h"

MK_FN(switchto_user_asm)
    mov sp, r0
    @ load user copies.
    ldm   sp, {r0-r14}^
    @ spsr and pc.
    add  sp, sp, #60
    rfe sp          @ rfe loads [pc and cpsr]
    asm_not_reached()


@ save all the registers in ascending order.
swi_trampoline:
    TRAMPOLINE_FULL_REGS(do_syscall)

@single_step_trampoline:
@    mov sp, #INT_STACK_ADDR
@    sub   lr, lr, #4
@    push {r0-r12,lr}
@    mov r0, lr
@    bl simple_single_step
@    pop {r0-r12,lr}
@    movs pc, lr

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


