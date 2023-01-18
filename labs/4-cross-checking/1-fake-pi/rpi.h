#ifndef __RPI_H__
#define __RPI_H__
// begin defining our r/pi library.

/************************************************************
 * The following routines are written by us.  They are in 
 * start.s: look at the implementations --- they are not 
 * complicated!  
 *
 * They are used to prevent the compiler from reordering or 
 * removing operations.  gcc does not does not understand assembly 
 * and thus can't optimize it.
 */

#include <stdint.h> // uint32_t

// used to write device memory: do not use a raw store.
// writes the 32-bit value <v> to address <addr>:   
//     *(uint32_t *)addr = v;
void put32(volatile void *addr, uint32_t v);
// same, but takes <addr> as a uint32_t
void PUT32(uint32_t addr, uint32_t v);

// call this to annotate that we computed an unsigned 32-bit 
// integer from a device.  used for cross-checking.
uint32_t DEV_VAL32(uint32_t x);

// used to read device memory: do not use a raw dereference!
//
// returns the 32-bit value at <addr>:  
//   return *(uint32_t *)addr
uint32_t get32(const volatile void *addr);
// same but takes <addr> as a uint32_t
uint32_t GET32(uint32_t addr);

// asm routine that does nothing: used so compiler cannot 
// optimize away loops.
void nop(void);

// countdown 'ticks' cycles
static inline void delay(unsigned ticks) {
    while(ticks-- > 0)
        nop();
}

/************************************************************
 * GPIO routines: you will implement these.
 */
// different functions we can set GPIO pins to.
typedef enum {
    GPIO_FUNC_INPUT   = 0,
    GPIO_FUNC_OUTPUT  = 1,
    GPIO_FUNC_ALT0    = 4,
    GPIO_FUNC_ALT1    = 5,
    GPIO_FUNC_ALT2    = 6,
    GPIO_FUNC_ALT3    = 7,
    GPIO_FUNC_ALT4    = 3,
    GPIO_FUNC_ALT5    = 2,
} gpio_func_t;

// set GPIO function for <pin> (input, output, alt...).  settings for other
// pins should be unchanged.
void gpio_set_function(unsigned pin, gpio_func_t function);



// set <pin> to be an output pin.
void gpio_set_output(unsigned pin);

// set <pin> to input.
void gpio_set_input(unsigned pin);

// set GPIO <pin> on.
void gpio_set_on(unsigned pin);

// set GPIO <pin> off
void gpio_set_off(unsigned pin);

// set <pin> to <v> (v \in {0,1})
void gpio_write(unsigned pin, unsigned v);

// return the value of <pin>.
int gpio_read(unsigned pin);


void delay_cycles(unsigned  ncyc);
#endif
