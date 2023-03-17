// trivial user-level OS wrappers.  since we use hashes for all the 
// user programs, we try to make sure that additions to this file
// won't change them.    note, of course, that modifying an
// existing implementation will change the hashes.
//
// mostly provides ways to print, exit, fork, waitpid.
// 
// the main diff from Unix, besides having almost no syscalls ---
// is that the pid is a relative pid rather than an absolute 
// [e.g., the first process forked has pid = 1, second = 2, etc]
// so that hashes work out.  otherwise whatever processes have
// run before will lead to uninteresting differences.

#ifndef __LIBOS_H__
#define __LIBOS_H__

// hack: we do inlines so we can keep each file self-contained and not 
// dialating.

#include <stdint.h>
#include <stdarg.h>
#include "syscalls.h"

#define die(x...) do { output(x); sys_exit(1); } while(0)
#define panic(args...) do { output(args); sys_exit(1); } while(0)

// libos-asm.S: trampoline to call systemcalls: <sysno> is the 
// number (see syscalls.h) and there are optional arguments.
// currently won't handle more than 3.
int syscall_invoke_asm(uint32_t sysno, ...);

// cut these down.
#define sys_putc(x)         syscall_invoke_asm(SYS_PUTC, x)
#define sys_put_hex(x)      syscall_invoke_asm(SYS_PUT_HEX, x)
#define sys_put_int(x)      syscall_invoke_asm(SYS_PUT_INT, x)
#define sys_put_pid()       syscall_invoke_asm(SYS_PUT_PID)
#define sys_get_cpsr()      syscall_invoke_asm(SYS_GET_CPSR)
#define sys_get_mode()      (sys_get_cpsr() & 0b11111)
#define sys_exit(x)         syscall_invoke_asm(SYS_EXIT, x)
#define sys_fork()          syscall_invoke_asm(SYS_FORK)
#define sys_waitpid(pid)     syscall_invoke_asm(SYS_WAITPID, pid)

// allocate more memory in process: right now doesn't do much
// (e.g., you can use beyond this)
static inline void * sys_sbrk(long incr) { 
    return (void*)syscall_invoke_asm(SYS_SBRK, incr);
}

typedef unsigned size_t;

// #define die(x) do { user_putk(x); sys_exit(1); } while(0)
// https://clc-wiki.net/wiki/strncmp#Implementation
static inline int strncmp(const char* _s1, const char* _s2, size_t n) {
    const unsigned char *s1 = (void*)_s1, *s2 = (void*)_s2;
    while(n--) {
        if(*s1++!=*s2++)
            return s1[-1] - s2[-1];
    }
    return 0;
}

// simplest routine to put a string.  mainly useful for minimizing
// the code that runs when debugging weird errors.
static inline void user_putk(const char *msg) {
    while(*msg)
        sys_putc(*msg++);
}

// simple printk that only takes %x as a format 
// we need a seperate routine since when actually at user level
// can't call kernel stuff.
//
// the trouble is: each time you change this, you get a different
// hash.
static inline void output(const char *fmt, ...) {
    unsigned chr;
    va_list ap;

    va_start(ap, fmt);
    while((chr = *fmt++)) {
        switch(chr) {
        case '%':
            switch((chr = *fmt++)) {
            case 'x': sys_put_hex(va_arg(ap, unsigned)); break;
            case 'd': sys_put_int(va_arg(ap, int)); break;
            default: die("bad character\n"); break;
            } 
            break;
        case '$':
            if(strncmp(fmt, "pid", 3) != 0)
                panic("unknown format: <%s>\n", fmt);
            sys_put_pid();
            fmt += 3;
            break;
        default:
            sys_putc(chr);
        }
    }
    va_end(ap);
}

// should remove this: asking for trouble.
#include <stdint.h>

// should remove this: asking for troube to inlude memset 
// from libpi
void *memset(void *dst, int c, size_t n);

// trivial bump allocator (no free).
void *talloc(unsigned n);

#define debug(msg, args...) \
    (output)("%s:%s:%d:DEBUG:" msg, __FILE__, __FUNCTION__, __LINE__, ##args)

#define assert(bool) do {                                   \
    if((bool) == 0) {                                       \
        debug("ERROR: Assertion `%s` failed.\n", #bool);    \
        sys_exit(1);                                        \
    }                                                       \
} while(0)
#endif
