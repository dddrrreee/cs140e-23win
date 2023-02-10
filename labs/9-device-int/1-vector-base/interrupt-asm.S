@ hacked code for testing vector base code and testing the
@ difference between the position-independent "generic" method
@ of last lab (that used the ldr instruction) with the faster
@ method of a relative branch (so: no memory load, no cache miss,
@ also, the ldr will commonly = a misprediction).
#include "rpi-asm.h"

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@ support to do a single system call, <sys_plus_1>

@ our trivial, kernel-level single system call implementation
@ that adds 1 to <r0> and returns.
@
@ this is about the minimum overhead for a call we can
@ have since there's no (1) saving/restoring of registers,
@ (2) additional jumps or (3) sanity checking.
@
@ NOTE: we will trash SUPER mode's <lr> so the caller 
@ has to push{lr}/pop{lr}
@
MK_FN(sys_plus1_handler)
    add r0, r0, #1
    movs pc, lr

@ trivial "user-level" client callsite that invokes 
@ <sys_plus1_handler> system call using a SWI 
@ instruction. 
@
@ NOTE: we are running at SUPER level today, as is the SWI handler:
@ thus we have to push{lr}/pop{lr} since otherwise the swi
@ exception would trash these.
@
@ for speed: you can also inline this, just be sure to specify that
@ the code clobbers all the caller (including lr)
MK_FN(sys_plus1)
    push {lr}
    swi 1
    pop {lr}
    bx lr

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@ original interrupt table from 
@    5-interrupts/0-timer-int:interrupt-asm.S
@
@ only change is that we call our sys_plus1 for SWI exceptions
@ and die on everything else.
@
.globl _interrupt_vector_orig
.align 5
_interrupt_vector_orig:
  ldr pc, _reset_asm
  ldr pc, _undefined_instruction_asm
  ldr pc, _software_interrupt_asm
  ldr pc, _prefetch_abort_asm
  ldr pc, _data_abort_asm
  ldr pc, _reset_asm
  ldr pc, _interrupt_asm
fast_interrupt_asm:
  sub   lr, lr, #4 @First instr of FIQ handler
  push  {lr}
  push  {r0-r12}
  mov   r0, lr              @ Pass old pc
  bl    fast_interrupt_vector    @ C function
  pop   {r0-r12}
  ldm   sp!, {pc}^

@ the <unhandled_*> trampolines are in 
@       <staff-src/unhandled-exception.S>
_reset_asm:                   .word unhandled_reset
_undefined_instruction_asm:   .word unhandled_undefined_instruction
_software_interrupt_asm:      .word sys_plus1_handler
_prefetch_abort_asm:          .word unhandled_prefetch_abort
_data_abort_asm:              .word unhandled_data_abort
_interrupt_asm:               .word unhandled_interrupt

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@ slight rewrite of the above:
@  - since we don't have to copy the interrupt table can get rid
@    of it and jump right to the trampoline code. 

@ roughly our original approach
@ the unhandled trampolines are in 
@       <staff-src/unhandled-exception.S>
.align 5
.globl _interrupt_vector_slow
_interrupt_vector_slow:
  ldr pc, =unhandled_reset
  ldr pc, =unhandled_undefined_instruction
  ldr pc, =sys_plus1_handler
  ldr pc, =unhandled_prefetch_abort
  ldr pc, =unhandled_data_abort
  ldr pc, =unhandled_interrupt

@ TODO: change the SWI call to do a relative branch <b> 
@ to sys_plus1_handler --- can do this since we don't have 
@ to copy the vector anywhere.
.align 5
.globl _interrupt_vector_fast
_interrupt_vector_fast:
  ldr pc, =unhandled_reset
  ldr pc, =unhandled_undefined_instruction
  ldr pc, =unhandled_swi
  ldr pc, =unhandled_prefetch_abort
  ldr pc, =unhandled_data_abort
  ldr pc, =unhandled_interrupt
