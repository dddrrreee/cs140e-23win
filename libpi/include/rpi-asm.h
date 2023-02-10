#ifndef __RPI_ASM_H__
#define __RPI_ASM_H__
// only include this in .S files.

#include "rpi-constants.h"

// declare an ASM routine.
#define MK_FN(fn_name)     \
.globl fn_name;             \
fn_name:

// declare a weak global symbol
#define MK_WEAK_SYM(name)   .weak name; name

// used to make a cp15 function: clear the input "read-only" register
#define MK_CP15_FUNC(name, macro_name)           \
.globl name;                                \
name:                                       \
    CLR(r0);                                 \
    macro_name(r0);                          \
    bx lr

// used to clear register before CP15 operation.
#define CLR(reg) mov reg, #0 

#define prefetch_flush(reg)     \
    mov reg, #0;                \
    mcr p15, 0, reg, c7, c5, 4

// cpp hack to turn argument into a string literal.
#define STRING2(x) #x
#define STRING(x) STRING2(x)

// we do some iffy hacks to make a concatenated
// string with "file:lineno:msg" and pass it to 
// the asm_todo_helper.
#define asm_call_helper_w_msg(fn, msg)  \
    mov sp, #INT_STACK_ADDR;        \
    sub r0, pc, #4;                 \
    /* grab the string address */   \
    add r1, pc, #0;                 \
    b fn;                           \
    .ascii  __FILE__;               \
    .ascii  ":";                    \
    .ascii  STRING(__LINE__);       \
    .ascii  ":<";                   \
    .ascii  #msg;                   \
    .ascii  ">";                    \
    .ascii  "\000";                 \
    .align  2

// use in ASM to annotate TODOs for lab
#define asm_todo(msg)  \
    asm_call_helper_w_msg(asm_todo_helper, msg)

// use in ASM to annotate impossible to reach locations
#define asm_not_reached()  \
    asm_call_helper_w_msg(asm_not_reached_helper, NOT REACHED)

// use in ASM to annotate unimplemented code
#define asm_not_implemented()  \
    asm_call_helper_w_msg(asm_not_implemented_helper, NOT IMPLEMENTED)

#define asm_bad_exception(msg)  \
    asm_call_helper_w_msg(asm_bad_exception_helper, msg)

#endif
