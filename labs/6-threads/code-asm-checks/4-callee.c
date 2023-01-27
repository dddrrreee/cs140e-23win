// check if a register is caller or callee saved by automatically
// checking the machine code
//
// you need to implement <todo> and handle all registers from r0-r12
#include "rpi.h"

#define STRING(x) #x

// write this: should return 1 if it's caller saved.
// hint: what machine instruction should be at the 
// function's address?

static inline int is_caller(void (*fp)(void)) {
    todo("use its machine code to check if fp definitely saves no registers");
    todo("returns 1 if it's caller saved");
}

// generates a function that has a single inline assembly 
// statement that clobbers the given register
// [see the caller-callee note]
#define clobber_reg_gen(reg)            \
    static void clobber_ ## reg(void) { \
        asm volatile("" : : : STRING(reg));     \
    }                       

#define assert_caller(reg) do {                                 \
    if(!is_caller(clobber_ ## reg))                                         \
        panic("reg %s is not caller\n", STRING(reg));           \
    else                                                        \
        output("TRACE: expected: reg %s is caller\n", STRING(reg));     \
} while(0)

#define assert_callee(reg) do {                                 \
    if(is_caller(clobber_ ## reg))                                         \
        panic("reg %s is not callee\n", STRING(reg));           \
    else                                                        \
        output("TRACE: expected: reg %s is callee\n", STRING(reg));     \
} while(0)


// generate the clobber routines.
clobber_reg_gen(r0)
clobber_reg_gen(r1)
clobber_reg_gen(r2)
// todo: ... fill in the rest
clobber_reg_gen(r4)


// FILL this in
// put all the registers you *DO NOT* save here [ignore r13,r14,r15]
// these are caller saved
void check_cswitch_ignore_regs(void) {
    assert_caller(r0);
    assert_caller(r1);
    todo("add all your non-saved registers here");

    trace("ignore regs passed\n");
}

// put all the regs you do save here [callee saved] 
// ignore r13,r14,r15
void check_cswitch_save_regs(void) {
    assert_callee(r4);
    todo("add all your saved registers here");

    trace("saved regs passed\n");
}

void notmain() {
    check_cswitch_ignore_regs();
    check_cswitch_save_regs();
    trace("SUCCESS\n");
}
