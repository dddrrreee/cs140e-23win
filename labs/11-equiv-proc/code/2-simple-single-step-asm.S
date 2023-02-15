#include "rpi-asm.h"

swi_trampoline:
    push {r1-r12,r14}
    bl do_syscall
    pop {r1-r12,r14}
    movs pc, lr

single_step_trampoline:
    mov sp, #INT_STACK_ADDR
    sub   lr, lr, #4
    push {r0-r12,lr}
    mov r0, lr
    bl simple_single_step
    pop {r0-r12,lr}
    movs pc, lr

@ only handle single step.
.align 5; 
.globl simple_single_except
simple_single_except:
    b unhandled_reset
    b unhandled_undefined_instruction
    b swi_trampoline
    b single_step_trampoline
    b unhandled_data_abort
    b unhandled_interrupt
    b unhandled_fiq
