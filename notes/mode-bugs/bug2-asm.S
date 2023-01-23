#include "rpi-asm.h"

.global cpsr_get
cpsr_get:
    mrs  r0, cpsr
    bx lr

.global sp_get
sp_get:
    mov r0, sp
    bx lr

@ we pull the SUPER mode lr to SYSTEM.
@ this doesn't fix the problem.
@ why?  (hint: even more banked registers)
.global switch_to_system_bug
switch_to_system_bug:
    @ save lr to a non-banked register
    mov r0, lr

    mov r1,  #0b11111
    orr r1,r1,#(1<<7)    @ disable interrupts.
    msr cpsr, r1
    prefetch_flush(r1)

    @ jump to the saved version.
    @
    @ why don't we need to restore lr?
    blx r0
