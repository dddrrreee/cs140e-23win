#ifndef __TEST_INTERRUPTS_H__
#define __TEST_INTERRUPTS_H__
// aggregate all the test definitions and code.
//  search for "TODO" below for the 6 routines to write.
#include "rpi.h"
#include "rpi-interrupts.h"
#include "rpi-inline-asm.h"
#include "cycle-count.h"

// we provide this code: does global initialization.  
//   see <test-interrupts.c>
typedef void (*init_fn_t)(void);

// client interrupt handler: returns 0 if didn't handle anything.
typedef int (*interrupt_fn_t)(uint32_t pc);
void test_startup(init_fn_t init_fn, interrupt_fn_t int_fn);

// global state [bad form: but hopefully makes lab code more obvious]
extern volatile int n_interrupt;
enum { out_pin = 21, in_pin = 20 };
enum { N = 1024*32 };

// TODO: you write these two routines.
void falling_init(void);
int falling_handler(uint32_t pc);

extern volatile int n_falling;
static inline void falling_int_startup(void) {
    test_startup(falling_init, falling_handler);
}

// TODO: you write these two routines.
void rising_init(void);
int rising_handler(uint32_t pc);

extern volatile int n_rising;
static inline void rising_int_startup(void) {
    test_startup(rising_init, rising_handler);
}


// compose rise and fall.
static inline void rise_fall_init(void) {
    // standarize: call rise first, then fall.
    rising_init();
    falling_init();
    // make sure in known state.
    assert(gpio_read(in_pin) == 1);
}
static inline int rise_fall_handler(uint32_t pc) {
    // NOTE: C short circuits: use "|" not "||"!
    return rising_handler(pc) 
        | falling_handler(pc);
}
static inline void rise_fall_int_startup(void) {
    test_startup(rise_fall_init, rise_fall_handler);
}

// TODO: you write these two routines using 
//    <5-interrupt/0-timer-int:timer.c>
void timer_test_init(void);
int timer_test_handler(uint32_t pc);
static inline void timer_int_startup(void) {
    test_startup(timer_test_init, timer_test_handler);
}


// compose rise and fall and timer.
static inline void rise_fall_timer_init(void) {
    rise_fall_init();
    timer_test_init();
}
static inline int 
rise_fall_timer_handler(uint32_t pc) {
    // NOTE: C short circuits: use "|" not "||"!
    return rise_fall_handler(pc) 
        | timer_test_handler(pc);
}
static inline void rise_fall_timer_int_startup(void) {
    test_startup(rise_fall_timer_init, rise_fall_timer_handler);
}

#endif
