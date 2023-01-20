/*
    engler: cs140: use cute gcc hack to figure out caller/callee

    compile using:
        arm-none-eabi-gcc -O2 -march=armv6 -c caller-callee.c -o caller-callee.o
        arm-none-eabi-objdump -d   caller-callee.o > caller-callee.list

   problem: the ARM eabi specifies for *some* of the registes which are caller and which
   are callee-saved.  for others, it allows either to hold.  it's unclear in general
   what gcc does (and it appears it can make different choices depending on options).

   so, our problem: for the specific gcc and specific compiler flags, which polarity
   are different registers? 
   
   solution, a nice trick from:
       https://stackoverflow.com/questions/261419/what-registers-to-save-in-the-arm-c-calling-convention
   is to use inline assembly, tell gcc that all registers are clobbered, and 
   then see which it decides to save.  

   one issue: those it *does not* save are *definitely*
   caller-saved (barring a compiler bug).  however, the others might or might not be 
   callee-saved because redundant saving isn't incorrect, just slow.  so we rely
   on high compiler optimization.   also it does appear that it handles lr weirdly.

    8d50:   e92d4ff0    push    {r4, r5, r6, r7, r8, r9, sl, fp, lr}
    8d54:   e320f000    nop {0}
    8d58:   e8bd8ff0    pop {r4, r5, r6, r7, r8, r9, sl, fp, pc}

    https://developer.arm.com/documentation/ihi0042/f/
    r0-r3 are the argument and scratch registers; r0-r1 are also the result registers
    r4-r8 are callee-save registers
    *r9 might be a callee-save register or not 
       (on some variants of AAPCS it is a special register)
    r10-r11 are callee-save registers
    *r12-r15 are special registers --- r12 seems to be a caller saved.

 */
void callee_regs(void) {
    // disassemble and look at what the compiler saves.  we don't test
    // r13 (sp) or r15 (pc).
    asm volatile ( "nop" : : : 
        "r0", "r1", "r2",  "r3",  "r4",  "r5",  "r6", 
        "r7", "r8", "r9", "r10", "r11", "r12", "r14");
}

// we mainly need to figure out if the compiler treats {r9, r12} as 
// callee or caller.  we check them individually.
void check_r9(void) {
    // should have push {r9}
    asm volatile ( "nop" : : : "r9");
}
void check_r12(void) {
    // should have nop
    asm volatile ( "nop" : : : "r12");
}

// verify that {r0-r3,r12} are caller: should just be a nop [it is]
void check_caller(void) {
    asm volatile ( "nop" : : : "r0", "r1", "r2", "r3", "r12");
}

// verify that {r4-r11} are callee
// <check_callee>:
//  8d74:   e92d0df0    push    {r4, r5, r6, r7, r8, sl, fp}
//  8d78:   e320f000    nop {0}
//  8d7c:   e8bd0df0    pop {r4, r5, r6, r7, r8, sl, fp}
//  8d80:   e12fff1e    bx  lr
void check_callee(void) {
    // should have a push and pop of all of these:
    // r10=sl
    // r11=fp
    // r12=ip
    // r13=sp
    // r14=ir
    // r15=pc
    asm volatile ( "nop" : : : "r4", "r5", "r6", "r7", "r8", "r9", "r10", "r11");
}
