@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@ user side of system calls.
@
#include "rpi-asm.h"

@ save lr b/c hardware will trash it (since we are at SUPER initially)
MK_FN(syscall_invoke_asm)
    push {lr}   @ we won't need these at user level.
    swi 0
    pop {lr}    @ we won't need these at user level.
    bx lr
