#include "rpi-asm.h"

.global cpsr_get
cpsr_get:
    mrs  r0, cpsr
    bx lr

.global sp_get
sp_get:
    mov r0, sp
    bx lr

@ jump to routine held in r0, with stack in r1
.global run_fn_at_system
run_fn_at_system:
    @ SYSTEM mode + disable interrupts
    msr cpsr, #(0b11111 | (1<<7))
    prefetch_flush(r2)

    mov sp, r1
    blx r0

    @ if we return, reboot
    bl  clean_reboot
