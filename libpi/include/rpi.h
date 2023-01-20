/*
 * General functions we use.  These could be broken into multiple small
 * header files, but that's kind of annoying to context-switch through,
 * so we put all the main ones here.
 */
#ifndef __RPI_H__
#define __RPI_H__

#define RPI_COMPILED


// We are running without an OS, but these will get pulled from gcc's include's,
// not your laptops.
// 
// however, we don't want to do this too much, since unfortunately header files
// have a bunch of code we cannot run, which can lead to problems.
//
// XXX: These are dangerous since we are not doing any initialization (e.g., of
// locale).  
#include <stddef.h>
#include <stdint.h>
#include <stdarg.h>


/*****************************************************************************
 * output routines.
 */

// put a single char
int rpi_putchar(int c);

// emit a single string.
int putk(const char *msg);

// printf with a lot of restrictions.
int printk(const char *format, ...);

// vprintf with a lot of restrictions.
int vprintk(const char *fmt, va_list ap);

/*****************************************************************************
 * uart routines: you will implement these.
 */

// initialize [XXX: we should take a baud rate?]
void uart_init(void);
// disable
void uart_disable(void);

// get one byte from the uart
int uart_getc(void);
// put one byte on the uart
void uart_putc(unsigned c);

// returns -1 if no byte, the value otherwise.
int uart_getc_async(void);

// 0 = no data, 1 = at least one byte
int uart_has_data(void);
// 0 = no space, 1 = space for at least 1 byte
int uart_can_putc(void);

// flush out the tx fifo
void uart_flush_tx(void);

/***************************************************************************
 * simple timer functions.
 */

// delays for <ticks> (each tick = a few cycles)
void delay_cycles(unsigned ticks) ;

// delay for <us> microseconds.
void delay_us(unsigned us) ;

// delay for <ms> milliseconds
void delay_ms(unsigned ms) ;

// returns time in usec.
// NOTE: this can wrap around!   do not do direct comparisons.
// this does a memory barrier.
unsigned timer_get_usec(void) ;

// no memory barrier.
unsigned timer_get_usec_raw(void);

/****************************************************************************
 * Reboot the pi smoothly.
 */

// reboot the pi.
void rpi_reboot(void) __attribute__((noreturn));

// reboot after printing out a string to cause the unix my-install to shut down.
void clean_reboot(void) __attribute__((noreturn));

// user can provide an implementation: will get called during reboot.
void reboot_handler(void);

/*****************************************************************************
 * memory related helpers
 */

// memory barrier.
void dmb(void);
// sort-of write memory barrier (more thorough).  dsb() >> dmb().
void dsb(void);
// use this if you need a device memory barrier.
void dev_barrier(void);

/*****************************************************************************
 * Low-level code: you could do in C, but these are in assembly to defeat
 * the compiler.
 */
// *(unsigned *)addr = v;
void PUT32(unsigned addr, unsigned v);
void put32(volatile void *addr, unsigned v);

// *(unsigned *)addr
unsigned GET32(unsigned addr);
unsigned get32(const volatile void *addr);

// jump to <addr>
void BRANCHTO(unsigned addr);

// a no-op routine called to defeat the compiler.
void dummy(unsigned);
void nop(void);


#   include "demand.h"
#ifndef RPI_UNIX
#   define asm_align(x)    asm volatile (".align " _XSTRING(x))

    // called for testing.
    static inline uint32_t DEV_VAL32(uint32_t x) { return x; }
#else
#   include <string.h>
#   include <stdlib.h>
#   include <assert.h>

    // it's gross that we have to add this.  what should do about this?
#   include "fake-pi.h"

    // call this to annotate that we computed an unsigned 32-bit 
    // integer from a device.
    uint32_t DEV_VAL32(uint32_t x);
#endif

// entry point definition
void notmain(void);

/*********************************************************
 * some gcc helpers.
 */

// gcc memory barrier.
#define gcc_mb() asm volatile ("" : : : "memory")

// from linux --- can help gcc make better code layout
// decisions.  can sometimes help when we want nanosec
// accurate code.
//
// however: leave these til the last thing you do.
//
// example use:
//   if(unlikely(!(p = kmalloc(4))))
//      panic("kmalloc failed\n");
#define likely(x)       __builtin_expect((x),1)
#define unlikely(x)     __builtin_expect((x),0)

#include "gpio.h"
#include "rpi-constants.h"
// any extra prototypes you want to add
#include "your-prototypes.h"

#endif
