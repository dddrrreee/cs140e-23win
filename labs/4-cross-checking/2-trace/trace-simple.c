// part 1: trace GET32/PUT32 (also: get32, put32).
//
// extension: add a simple log for capturing
//
//  without capture it's unsafe to call during UART operations since infinitely
//  recursive and, also, will mess with the UART state.  
//
//  record the calls in a log (just declare a statically sized array until we get malloc)
//  for later emit by trace_dump()
//
// extension: keep adding more functions!  
//      often it's useful to trace at a higher level, for example, doing 
//      uart_get() vs all the put's/get's it does, since that is easier for
//      a person to descipher.  or, you can do both:
//          -  put a UART_PUTC_START in the log.
//          - record the put/gets as normal.
//          - put a UART_PUTC_STOP in the log.
//  
#include "rpi.h"
#include "trace.h"

// gross that these are not auto-consistent with GET32 in rpi.h
unsigned __wrap_GET32(unsigned addr);
unsigned __real_GET32(unsigned addr);
void __wrap_PUT32(unsigned addr, unsigned val);
void __real_PUT32(unsigned addr, unsigned val);

// simple state machine to track what set of options we're called with.
enum {
    TRACE_OFF = 0,
    TRACE_ON,
    TRACE_SKIP   // if running w/o buffering, stop printing trace
};
static int state = TRACE_OFF;

// start tracing : 
//   <buffer_p> = 0: tells the code to immediately print the 
//                   trace.  
//   <buffer_p> != 0 defers the printing until <trace_stop> is called.
//
//  is an error: if you were already tracing.
void trace_start(int buffer_p) {
    assert(state == TRACE_OFF);
    state = TRACE_ON;
} 

// stop tracing
//  - error: if you were not already tracing.
void trace_stop(void) {
    assert(state == TRACE_ON);
    state = TRACE_OFF;
    // we would print things out if output was being buffered.
}

// call these to emit so everyone can compare!
static void emit_put32(uint32_t addr, uint32_t val) {
    printk("TRACE:PUT32(0x%x)=0x%x\n", addr, val);
}
static void emit_get32(uint32_t addr, uint32_t val) {
    printk("TRACE:GET32(0x%x)=0x%x\n", addr, val);
}

// the linker will change all calls to GET32 to call __wrap_GET32
void __wrap_PUT32(unsigned addr, unsigned val) {
    // XXX: implement this function!
    unimplemented();
}

// the linker will change all calls to GET32 to call __wrap_GET32
unsigned __wrap_GET32(unsigned addr) {
    unsigned v = 0;
    // implement this function!
    unimplemented();
    return v;
}
