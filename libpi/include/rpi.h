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
#include <string.h>


/*****************************************************************************
 * output routines.
 */


// let the user override the system putchar routine.

typedef int (*rpi_putchar_t)(int chr);
// put a single char
extern rpi_putchar_t rpi_putchar;

// override the routine.
rpi_putchar_t rpi_putchar_set(rpi_putchar_t putc);
#define rpi_set_putc rpi_putchar_set


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
int uart_get8(void);
// put one byte on the uart:
// returns < 0 on error.
int uart_put8(uint8_t c);

int uart_hex(unsigned h);

// returns -1 if no byte, the value otherwise.
int uart_get8_async(void);

// 0 = no data, 1 = at least one byte
int uart_has_data(void);

// 0 = no space, 1 = space for at least 1 byte
int uart_can_put8(void);

// flush out the tx fifo
void uart_flush_tx(void);

/***************************************************************************
 * simple timer functions.
 */

// delays for <ticks> (each tick = a few cycles)
void delay_cycles(uint32_t ticks) ;

// delay for <us> microseconds.
void delay_us(uint32_t us) ;

// delay for <ms> milliseconds
void delay_ms(uint32_t ms) ;

// returns time in usec.
// NOTE: this can wrap around!   do not do direct comparisons.
// this does a memory barrier.
uint32_t timer_get_usec(void) ;

// no memory barrier.
uint32_t timer_get_usec_raw(void);

/****************************************************************************
 * Reboot the pi smoothly.
 */

// reboot the pi.
void rpi_reboot(void) __attribute__((noreturn));

// reboot after printing out a string to cause the unix my-install to shut down.
void clean_reboot(void) __attribute__((noreturn));

// user can provide an implementation: will get called during reboot.
void reboot_callout(void);

/*****************************************************************************
 * memory related helpers
 */

// memory barrier.
void dmb(void);
// sort-of write memory barrier (more thorough).  dsb() >> dmb().
void dsb(void);
// use this if you need a device memory barrier.
void dev_barrier(void);

/*******************************************************************************
 * simple memory allocation: no free, just have to reboot().
 */

// returns 0-filled memory.
void *kmalloc(unsigned nbytes) ;
void *kmalloc_aligned(unsigned nbytes, unsigned alignment);

// initialize and set where the heap starts and give a maximum
// size in bytes
void kmalloc_init_set_start(void *addr, unsigned max_nbytes);
static inline void kmalloc_init(void) {
    unsigned MB = 1024*1024;
    kmalloc_init_set_start((void*)MB, 64*MB);
}

// return pointer to the first free byte.  used for
// bounds checking.
void *kmalloc_heap_ptr(void);
// pointer to initial start of heap
void *kmalloc_heap_start(void);
// pointer to end of heap
void *kmalloc_heap_end(void);

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

// *(volatile uint8_t *)addr = x;
void put8(volatile void *addr, uint8_t x);
void PUT8(uint32_t addr, uint8_t x);

uint8_t GET8(unsigned addr);
uint8_t get8(const volatile void *addr);

// jump to <addr>
void BRANCHTO(unsigned addr);

// a no-op routine called to defeat the compiler.
void dummy(unsigned);
void nop(void);


/* #include <string.h> */

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

// provide your own implementation if you want to 
// do something during a busy wait.
void rpi_wait(void);

// enable branch and icache
void caches_enable(void);
// disable branch and icache
void caches_disable(void);
int caches_is_enabled(void);


int memiszero(const void *_p, unsigned n);


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


void custom_loader(void);
#endif
