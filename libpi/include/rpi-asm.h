#ifndef __RPI_ASM_H__
#define __RPI_ASM_H__
// only include this in .S files.

#include "rpi-constants.h"

#define MK_FN(fn_name)     \
.globl fn_name;             \
fn_name:


// used to make a cp15 function: clear the input "read-only" register
#define MK_CP15_FUNC(name, macro_name)           \
.globl name;                                \
name:                                       \
    CLR(r0);                                 \
    macro_name(r0);                          \
    bx lr


// used to clear register before CP15 operation.
#define CLR(reg) mov reg, #0 

#define prefetch_flush(reg) mov reg, #0; mcr p15, 0, reg, c7, c5, 4

// we load a new stack pointer: keep an eye on this in future.
#define asm_not_reached()           \
    mov sp, #INT_STACK_ADDR;    \
    mov r0, pc;                 \
    b asm_not_reached_helper

#define asm_not_implemented()       \
    mov sp, #INT_STACK_ADDR;    \
    mov r0, pc;                 \
    b asm_not_implemented_helper

#endif
