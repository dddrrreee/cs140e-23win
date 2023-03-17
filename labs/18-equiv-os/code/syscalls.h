#ifndef __SYSCALLS_H__
#define __SYSCALLS_H__

// should keep working if you change this number.
#define SYS_EXIT            0
#define SYS_PUTC            1
#define SYS_GET_CPSR        2

/* print a single hex number (in reg <r2>) using a string <r1> */
#define SYS_PUT_HEX       3
#define SYS_PUT_INT       4
#define SYS_PUT_PID       5

#define SYS_SBRK       6
#define SYS_FORK       7
#define SYS_WAITPID       8
#endif
