#include "rpi-asm.h"

.global cpsr_get
cpsr_get:
    mrs  r0, cpsr
    bx lr

@ switch to SYSTEM mode (0b11111)
@ and return. 
@
@ the way we do it doesn't work of course.
@ hint: banked registers. 
.global switch_to_system_bug
switch_to_system_bug:
    mov r1,  #0b11111
    orr r1,r1,#(1<<7)    @ disable interrupts.
    msr cpsr, r1
    prefetch_flush(r1)
    bx lr
