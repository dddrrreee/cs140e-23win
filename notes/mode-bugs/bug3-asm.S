#include "rpi-asm.h"

.global cpsr_get
cpsr_get:
    mrs  r0, cpsr
    bx lr

.global sp_get
sp_get:
    mov r0, sp
    bx lr

@ switch to stack address passed in r0
@
@ bug: why?  hint: where is the previous 
@ stuff in caller stored?
.global switch_to_system_w_stack
switch_to_system_w_stack:
    @ save lr to non banked.
    mov r1, lr

    @ set SYSTEM mode + disable interrupts
    msr cpsr, #(SYS_MODE | (1<<7))
    prefetch_flush(r2)

    @ load sp from first argument
    mov sp, r0
    @ jump back to original lr
    blx r1
