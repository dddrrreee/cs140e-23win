// trivial little fake r/pi system that will allow you to debug your
// gpio.c code.  can compile on your laptop and then run the code there
// too:
//      # compile using your normal compiler.
//      % gcc fake-pi.c gpio.c 1-blink.c
//      % ./a.out
//      calling pi code
//      GET32(20200008) = 643c9869
//      ...
//      PUT32(2020001c) = 100000
//      PUT32(20200028) = 100000
//      pi exited cleanly
//
// some nice things:
//  1. use a debugger;
//  2. by comparing the put/get values can check your code against other
//     people (lab 3)
//  3. you have memory protection, so null pointer writes get detected.
//  4. can run with tools (e.g., valgrind) to look for other errors.
//
// while the fake-pi is laughably simple and ignores more than it handles,
// it's still useful b/c:
//  1. most of your code is just straight C code, which runs the same on
//     your laptop and the pi.
//  2. the main pi specific thing is what happens when you read/write
//     gpio addresses.   given how simple these are, we can get awa
//     with just treating them as memory, where the next read returns
//     the value of the last write.  for fancier hardware, or for 
//     a complete set of possible GPIO behaviors we would have to do
//     something fancier.   
//
//     NOTE: this problem of how to model devices accurately is a 
//     big challenge both for virtual machines and machine simulators.
//
// it's good to understand what is going on here.  both why it works,
// and when you can use this kind of trick in other places.
//      - one interesting thing: we can transparently take code that
//      you wrote explicitly to run bare-metal on the pi ARM cpu,
//      and interact with the weird broadcom chip and run it on
//      your laptop, which has neither, *without making any change!*
//

// there is no assembly or other weird things in rpi.h so we can
// include and use it on your laptop even tho it is intended for
// the pi.

#include "rpi.h"
#undef output
#undef debug
#undef panic
#undef trace

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>


#include "fake-pi.h"

/***********************************************************************
 * some macros to make error reporting easier.
 */



// print output
#define output(msg, args...) \
    do { printf(msg, ##args ); fflush(stdout); } while(0)

// print output with file/function/line
#define debug(msg...) do {                                           \
    output("%s:%s:%d:", __FILE__, __FUNCTION__, __LINE__);          \
    output(msg);                                                  \
} while(0)

// panic and die with an error message.
#define panic(msg, args...) \
    do { debug("PANIC:" msg, ##args); exit(1); } while(0)


// emit a trace statement -- these are the outputs that you compare
// run to run to see that the code doesn't change.

// some programs run forever -- truncate after <max_trace> statements
enum { max_trace = 128 };
unsigned ntrace = 0;

static unsigned trace_on_p = 0, trace_ops = 0;
static inline void trace_on(void) { trace_on_p = 1; }
static inline void trace_off(void) { trace_on_p = 0; }

#define trace(msg, args...) do {                                        \
    if(trace_on_p) {                                                    \
        output("TRACE:%d: " msg, trace_ops++, ##args);                                   \
        if(ntrace++ >= max_trace) {                                     \
            output("TRACE: exiting after %d trace statements\n", ntrace);\
            fake_pi_exit();                                             \
        }                                                               \
    }                                                                   \
} while(0)


/***********************************************************************
 * a tiny fake simulator that traces all reads and writes to device
 * memory.
 *
 * main pi-specific thing is a tiny model of device
 * memory: for each device address, what happens when you
 * read or write it?   in real life you would build this
 * model more succinctly with a map, but we write everything 
 * out here for maximum obviousness.
 */

#define GPIO_BASE 0x20200000

// the locations we track.
enum {
    gpio_fsel0 = (GPIO_BASE + 0x00),
    gpio_fsel1 = (GPIO_BASE + 0x04),
    gpio_fsel2 = (GPIO_BASE + 0x08),
    gpio_fsel3 = (GPIO_BASE + 0x0c),
    gpio_set0  = (GPIO_BASE + 0x1C),
    gpio_clr0  = (GPIO_BASE + 0x28),
    gpio_lev0  = (GPIO_BASE + 0x34)
};

// the value for each location.
static unsigned 
        gpio_fsel0_v,
        gpio_fsel1_v,
        gpio_fsel2_v,
        gpio_fsel3_v,
        gpio_set0_v,
        gpio_clr0_v;


// same, but takes <addr> as a uint32_t
void PUT32(uint32_t addr, uint32_t v) {
    if(!trace_on_p)
        output("initializing PUT32(0x%x) = 0x%x\n", addr, v);
    trace("PUT32(0x%x) = 0x%x\n", addr, v);
    switch(addr) {
    case gpio_fsel0: gpio_fsel0_v = v;  break;
    case gpio_fsel1: gpio_fsel1_v = v;  break;
    case gpio_fsel2: gpio_fsel2_v = v;  break;
    case gpio_fsel3: gpio_fsel3_v = v;  break;
    case gpio_set0:  gpio_set0_v  = v;  break;
    case gpio_clr0:  gpio_clr0_v  = v;  break;
    case gpio_lev0:  panic("illegal write to gpio_lev0!\n");
    default: panic("write to illegal address: %x\n", addr);
    }
}
// same as PUT32 but takes a pointer.
void put32(volatile void *addr, uint32_t v) {
    PUT32((uint32_t)(uint64_t)addr, v);
}

uint32_t DEV_VAL32(uint32_t x) {
    trace("DEV_VAL32=0x%x\n", x);
    return x;
}

// same but takes <addr> as a uint32_t
uint32_t GET32(uint32_t addr) {
    unsigned v;
    switch(addr) {
    case gpio_fsel0: v = gpio_fsel0_v; break;
    case gpio_fsel1: v = gpio_fsel1_v; break;
    case gpio_fsel2: v = gpio_fsel2_v; break;
    case gpio_fsel3: v = gpio_fsel3_v; break;
    case gpio_set0:  v = gpio_set0_v;  break;
    case gpio_clr0:  v = gpio_clr0_v;  break;
    // to fake a changing environment, we want gpio_lev0 to 
    // change --- so we return a random value for (which
    // will be roughly uniform random for a given bit).
    // you could bias these as well or make them more 
    // realistic by reading from a trace from a run on 
    // the raw hardware, correlating with other pins or 
    // time or ...
    case gpio_lev0:  v = fake_random();  break;
    default: panic("read of illegal address: %x\n", addr);
    }
    trace("GET32(0x%x) = 0x%x\n", addr,v);
    return v;
}
// same as GET32 but takes a pointer.
uint32_t get32(const volatile void *addr) {
    return GET32((uint32_t)(uint64_t)addr);
}

// don't need to do anything.  would be better to not have
// delay() in the header so we could do our own version and
// (for example) print out the number of ticks used.
void nop(void) {
}

void fake_pi_exit(void) {
    output("TRACE: pi exited cleanly: %d calls to random\n", fake_random_calls());
    exit(0);
}

void delay_cycles(unsigned ncycles) {
    trace("delaying %d cycles\n", ncycles);
}

// initialize "device memory" and then call the pi program
int main(int argc, char *argv[]) {
    void notmain(void);

    if(argc == 1)  {
        fake_random_init();
    } else if(argc == 2) {
        unsigned seed = atoi(argv[1]);
        if(seed == 0)
            panic("seed was <%s>: should be a non-zero number\n", argv[1]);
        output("TRACE: initializing fake_random to %x\n", seed);
        fake_random_seed(seed);
    } else
        panic("invalid number of args: %d\n", argc);

    // initialize "device memory" to random values.
    // extension: do multiple iterations, initializing to
    // different values each time.
    //
    // we do it raw vs put32 so nothing gets printed.
    trace_off();
    PUT32(gpio_fsel0, fake_random());
    PUT32(gpio_fsel1, fake_random());
    PUT32(gpio_fsel2, fake_random());
    PUT32(gpio_fsel3, fake_random());
    PUT32(gpio_set0,  fake_random());
    PUT32(gpio_clr0,  fake_random());
    trace_on();

    // extension: run in a subprocess to isolate
    // errors.
    trace("calling pi code\n");
    notmain();
    fake_pi_exit();
    return 0;
}
