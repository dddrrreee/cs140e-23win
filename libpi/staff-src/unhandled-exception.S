#include "rpi-asm.h"

#if 0
@ see libpi/include/asm-helpers.h
@ for <asm_bad_exception> macro: calls
@ C code with file/lineno and message.
MK_WEAK_SYM(unhandled_reset):
  asm_bad_exception(reset)
MK_WEAK_SYM(unhandled_undefined_instruction):
  asm_bad_exception(undefined instruction)
MK_WEAK_SYM(unhandled_swi):
  asm_bad_exception(software interrupt)
MK_WEAK_SYM(unhandled_prefetch_abort):
  asm_bad_exception(prefetch abort)
MK_WEAK_SYM(unhandled_data_abort):
  asm_bad_exception(data abort)
MK_WEAK_SYM(unhandled_interrupt):
  asm_bad_exception(interrupt)
#endif

@ save all the stuff we need, then call the given routine.
#define TRAMPOLINE(fn)              \
  mov sp, #INT_STACK_ADDR;          \
  push  {r0-r12,lr};                \
  mov   r0, lr;                     \
  bl    fn;                         \
  pop   {r0-r12,lr};                \
  movs    pc, lr


// keep the name of the trampoline private.
#define MK_TRAMPOLINE(name, offset, fn)     \
.weak name;                  \
name:                         \
    sub   lr, lr, # (offset);               \
    TRAMPOLINE(fn)

@ call the default handlers and blow up.  override if you
@ want to do something fancier.
MK_TRAMPOLINE(unhandled_reset,            4, reset_vector)
MK_TRAMPOLINE(unhandled_undefined_instruction,            4, undefined_instruction_vector)
MK_TRAMPOLINE(unhandled_prefetch_abort,   4, prefetch_abort_vector)
MK_TRAMPOLINE(unhandled_data_abort,       8, data_abort_vector)
MK_TRAMPOLINE(unhandled_interrupt,        4, int_vector)
MK_TRAMPOLINE(unhandled_swi,              4, syscall_vector)
MK_TRAMPOLINE(unhandled_fiq,              4, fiq_vector)

