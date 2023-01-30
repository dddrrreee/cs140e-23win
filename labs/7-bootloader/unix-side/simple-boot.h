#ifndef __UNIX_BOOT_H__
#define __UNIX_BOOT_H__
// helper routines; you should not have to modify this file.
// make sure you look at <get_op> and understand what it is doing.
#include "libunix.h"
#include "boot-crc32.h"
#include "boot-defs.h"

// used as a tracing hack.
enum { TRACE_FD = 21 };

#define boot_output(msg...) output("BOOT:" msg)

// helper tracing put/get routines: if you set 
//  <trace_p> = 1 
// you can see the stream of put/gets: makes it easy 
// to compare your bootloader to the ours and others.
//
// there are other ways to do this --- this is
// clumsy, but simple.
extern int trace_p;

static inline uint8_t
trace_get8(int fd) {
    uint8_t v = get_uint8(fd);
    if(trace_p)
        trace("GET8:%x\n", v);
    return v;
}

static inline uint32_t 
trace_get32(int fd) {
    uint32_t v = get_uint32(fd);
    if(trace_p)
        trace("GET32:%x\n", v);
    return v;
}

static inline void
trace_put8(int fd, uint8_t v) {
    if(trace_p)
        trace("PUT8:%x\n", v);
    put_uint8(fd, v);
}
static inline void
trace_put32(int fd, uint32_t v) {
    if(trace_p)
        trace("PUT32:%x\n", v);
    put_uint32(fd,v);
}

void simple_boot(int fd, const uint8_t *buf, unsigned n);

// always call this routine on the first 32-bit word in any message
// sent by the pi.
//
// hack to handle unsolicited <PRINT_STRING>: 
//  1. call <get_op> for the first uint32 in a message.  
//  2. the code checks if it received a <PRINT_STRING> and emits if so;
//  3. otherwise returns the 32-bit value.
//
// error:
//  - only call IFF the word could be an opcode (see <simple-boot.h>).
//  - do not call it on data since it could falsely match a data value as a 
//    <PRINT_STRING>.
static inline uint32_t 
get_op(int fd) {
    // we do not trace the output from PRINT_STRING so do not call the
    // tracing operations here except for the first word after we are 
    // sure it is not a <PRINT_STRING>
    uint32_t op = get_uint32(fd);
    if(op != PRINT_STRING) {
        if(trace_p)
            trace("GET32:%x\n", op);
        return op;
    }


    // never trace this code.
    boot_output("PRINT_STRING:");
    unsigned nbytes = get_uint32(fd);
    demand(nbytes < 512, pi sent a suspiciously long string);
    output("pi sent print: <");
    for(int i = 0; i < nbytes-1; i++)
        output("%c", get_uint8(fd));

    // eat the trailing newline to make it easier to compare output.
    uint8_t c = get_uint8(fd);
    if(c != '\n')
        output("%c", c);
    output(">\n");

    // attempt to get a non <PRINT_STRING> op.
    return get_op(fd);
}

// a helper routine that makes the code cleaner: call to compare
// the word you read with the one you expect.
//
// check that the expected value <exp> is equal the the value we <got>.
// on mismatch, drains the tty and echos (to help debugging) and then 
// dies.
static void ck_eq32(int fd, const char *msg, unsigned exp, unsigned got) {
    if(exp == got)
        return;

    // XXX: need to check: can there be garbage in the /tty when we
    // first open it?  If so, we should drain it.
    output("%s: expected %x, got %x\n", msg, exp, got);

    // after error: just echo the pi output so we can kind of see what is going
    // on.   <TRACE_FD> is used later.
    unsigned char b;
    while(fd != TRACE_FD && read(fd, &b, 1) == 1)
        fputc(b, stderr);
    panic("pi-boot failed\n");
}
#endif
