// engler, cs140e: simple utilities for using the cycle counters.
#ifndef __CYCLE_UTIL_H__
#define __CYCLE_UTIL_H__
#include "rpi.h"
#include "cycle-count.h"

// delay <n> cycles assuming we started counting cycles at time
// <start>.
static inline unsigned delay_ncycles(unsigned start, unsigned n) {
    // this handles wrap-around, but doing so adds two instructions,
    // which makes the delay not as tight.
    unsigned c;
    while(((c = cycle_cnt_read()) - start) < n)
        ;
    return c;
}

// write value <v> to GPIO <pin>: return when <ncycles> have passed since
// time <start>
//  
// can make a big difference to make an inlined version of GPIO_WRITE_RAW
// after you do: if you look at the generated assembly, there's some
// room for tricks.  also, remove the pin check.
static inline unsigned
write_cyc_until(unsigned pin, unsigned v, unsigned start, unsigned ncycles) {
    // GPIO_WRITE_RAW(pin,v);
    gpio_write(pin,v);
    return delay_ncycles(start,ncycles);
}   

// We don't seem to need this, so removing for the moment.  can
// drop in and experiment.
//
// we can't do direct loads and stores when faking things out, 
// so redefine it when running on Unix
// #ifdef RPI_UNIX
#if 0
#   define GPIO_READ_RAW gpio_read
#else
    // trying to eek out what we can in terms of speed so that we
    // do not get 
    static inline unsigned GPIO_READ_RAW(unsigned pin) {
        unsigned GPIO_BASE  = 0x20200000UL;
    
        volatile unsigned *gpio_lev0  = (void*)(GPIO_BASE + 0x34);
        unsigned off = (pin%32);
        // this can sign extend?
        return (*gpio_lev0 >> off) & 1;
    }
#endif

// wait until <pin>=<v> or until we spin for <ncycles>
static inline int 
wait_until_cyc(unsigned pin, unsigned v, unsigned s, unsigned ncycles) {
    while(1) {
        if(GPIO_READ_RAW(pin) == v)
            return 1;
        if((cycle_cnt_read() - s) >= ncycles)
            return 0;
    }
}

// usec: does not have to be that accurate since the time is just for timeout.
static inline int wait_until_usec(int pin, int v, unsigned timeout_usec) {
    if(GPIO_READ_RAW(pin) == v)
        return 1;
    unsigned start = timer_get_usec_raw();
    while(1) {
        // use GPIO_READ_RAW
        if(GPIO_READ_RAW(pin) == v)
            return 1;
        if((timer_get_usec_raw() - start) > timeout_usec)
            return 0;
    }
}
#endif
