/*
 * implement these.  ordered in difficulty.
 */
#include "rpi-asm.h"

MK_FN(check_push_asm)
    mov r1, sp      @ before
    push {r0}
    mov r0, sp      @ after
    bl after_push
    asm_not_reached()


@ do this so doesn't have to be defined in the tests.
.weak after_push
after_push:
    asm_not_reached();


@ manually set r4 to 4, r5 to 5 ... r12 to 12
#define IDENT_SET   \
    mov r4, #4;     \
    mov r5, #5;     \
    mov r6, #6;     \
    mov r7, #7;     \
    mov r8, #8;     \
    mov r9, #9;     \
    mov r10, #10;   \
    mov r11, #11;   \
    mov r12, #12

@ helper routine called pointer to memory (r0)
@   - that it does a push {r4-r12} into
@   - caller checks order
MK_FN(push_r4_r12_asm)
    @ save caller state
    push {r4-r12}   @ save the registers we will trash.
    mov r1, sp      @ save sp to scratch register


    @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    @ do the push with fixed values.

    @ move the passed in ptr (in r0) to sp
    mov sp, r0
    IDENT_SET       @ trash registers with known values
    push {r4-r12}   @ push to given memory block.
    mov r0, sp      @ return where sp ended up.

    @ done
    @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

    @ restore caller state
    mov sp, r1      @ restore sp
    pop {r4-r12}    @ restore trashed regs
    bx lr
    asm_todo(implement check_push_order_asm)


@ save just the callee and lr.
@ before doing so, write the number of
@ each reg to it.  e.g.:
@     mov r4, #4
@ this lets us check that things are
@ at the right offset.
@
@ then:
@   - push all the registers
@   - store the sp to the address in r0.
@   - call <after_writing_regs>
MK_FN(write_regs_to_stack)
    mov r4, #4
    mov r5, #5
    mov r6, #6
    mov r7, #7
    mov r8, #8
    mov r9, #9
    mov r10, #10
    mov r11, #11
    mov lr, #14

    @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    @ push all the callee
    @ store sp into the address held in r0
    asm_todo(implement this)
    @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

    mov r0, sp
    bl after_writing_regs
    asm_not_reached()

.weak after_writing_regs
after_writing_regs:
    asm_not_reached()
