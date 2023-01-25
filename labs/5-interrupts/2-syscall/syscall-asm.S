#include "rpi-asm.h"

MK_FN(syscall_hello)
    @ we are already at system level: running this will trash
    @ the lr, so we need to save it.
    push {lr}
    swi 1
    pop {lr}
    bx lr

@ your code should reject this call with an error.
MK_FN(syscall_illegal)
    push {lr}
    swi 2
    pop {lr}
    bx lr


@ look at notes/mode-bugs for examples on what to do
@    - code address passed in r0
@    - stack address passed in r1
@
@ switch to user level, run the address in <r0> using stack
@ <r1>
@
@ does not return
MK_FN(run_user_code_asm)
    asm_not_implemented()
