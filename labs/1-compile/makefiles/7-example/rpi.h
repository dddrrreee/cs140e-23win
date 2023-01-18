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

/**
 * Used to write to device memory.  Use this instead of a raw store.
 * Writes the 32-bit value <v> to address <addr>.
 *
 * Safer alternative to *(uint32_t *)addr = v;
 *
 * Like PUT32, but takes <addr> as a pointer.
 */
void put32(volatile void *addr, uint32_t v);

/**
 * Used to write to device memory.  Use this instead of a raw store.
 * Writes the 32-bit value <v> to address <addr>.
 *
 * Safer alternative to *(uint32_t *)addr = v;
 *
 * Like put32, but takes <addr> as an int.
 */
void PUT32(uint32_t addr, uint32_t v);

/**
 * Used to read from device memory.  Use this instead of a raw dereference.
 * Returns the 32-bit value at address <addr>.
 *
 * Safer alternative to *(uint32_t *)addr.
 *
 * Like GET32, but takes <addr> as a pointer.
 */
uint32_t get32(const volatile void *addr);

/**
 * Used to read from device memory.  Use this instead of a raw dereference.
 * Returns the 32-bit value at address <addr>.
 *
 * Safer alternative to *(uint32_t *)addr.
 *
 * Like get32, but takes <addr> as an int.
 */
uint32_t GET32(uint32_t addr);

/**
 * An assembly routine that does nothing.
 * The compiler cannot optimize away an external function call, so we can use
 * it to prevent the compiler optimizing away loops.
 */
void nop(void);

/**
 * Counts down <ticks> cycles.
 */
void delay_cycles(unsigned ticks);

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


/**
 * Configure GPIO <pin> as an output pin.
 */
void gpio_set_output(unsigned pin);

/**
 * Configure GPIO <pin> as an input pin.
 */
void gpio_set_input(unsigned pin);

/**
 * Set GPIO <pin> on.
 */
void gpio_set_on(unsigned pin);

/**
 * Set GPIO<pin> off.
 */
void gpio_set_off(unsigned pin);

/**
 * Set GPIO <pin> to <v>.
 *
 * <v> should be interepreted as a C "boolean"; i.e., 0 is false, anything else
 * is true.
 */
void gpio_write(unsigned pin, unsigned v);

/**
 * Read the current value of GPIO <pin>.
 * Returns either 0 or 1.
 */
int gpio_read(unsigned pin);

/***********************************************************
 * GPIO Pullup/Down routines; these aren't necessary for many things, but can be
 * useful.  They let an input pin have a "default" state when the pin isn't
 * connected, instead of randomly reading as 0 or 1.
 */

/**
 * Activate the pullup register on GPIO <pin>.
 *
 * GPIO <pin> must be an input pin.
 */
void gpio_set_pullup(unsigned pin);

/**
 * Activate the pulldown register on GPIO <pin>.
 *
 * GPIO <pin> must be an input pin.
 */
void gpio_set_pulldown(unsigned pin);

/**
 * Deactivate both the pullup and pulldown registers on GPIO <pin>.
 *
 * GPIO <pin> must be an input pin.
 */
void gpio_pud_off(unsigned pin);

#endif
