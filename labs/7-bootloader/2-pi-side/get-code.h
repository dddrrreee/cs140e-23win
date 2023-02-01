// bootloader interface and helpers.
//  
// much more robust than xmodem, which seems to have bugs in terms of 
// recovery with inopportune timeouts.
//
// You should not have to modify code in this header.
#ifndef __GETCODE_H__
#define __GETCODE_H__
#include "boot-crc32.h"  // has the crc32 implementation.
#include "boot-defs.h"   // protocol opcode values.

// returns 0 if failed, 1 if success: address to jump to is in
// <code_addr>.
//
// in the future: will make a version that places the code in 
// a buffer and returns it.
int get_code(uint32_t *code_addr);


/***************************************************************
 * helper routines.
 */

// return a 32-bit: little endien order.
//
// NOTE: as-written, the code will loop forever if data does
// not show up or the hardware dropped bytes (e.g. b/c 
// you didn't read soon enough)
//
// After you get the simple version working, you should fix 
// it by making a timeout version.
static inline uint32_t boot_get32(void) {
    uint32_t u = uart_get8();
        u |= uart_get8() << 8;
        u |= uart_get8() << 16;
        u |= uart_get8() << 24;
    return u;
}

// send 32-bits on the uart.
static inline void boot_put32(uint32_t u) {
    uart_put8((u >> 0)  & 0xff);
    uart_put8((u >> 8)  & 0xff);
    uart_put8((u >> 16) & 0xff);
    uart_put8((u >> 24) & 0xff);
}

// send a string <msg> to the unix side to print.  
// message format:
//  [PRINT_STRING, strlen(msg), <msg>]
//
// DANGER DANGER DANGER!!!  Be careful WHEN you call this!
// DANGER DANGER DANGER!!!  Be careful WHEN you call this!
// DANGER DANGER DANGER!!!  Be careful WHEN you call this!
// Why:
//   1. We do not have interrupts. 
//   2. The UART RX FIFO can only hold 8 bytes before it starts 
//      dropping them.   
//   3. So if you print at the same time the laptop sends data,
//      you will likely lose some, leading to weird bugs.  (multiple
//      people always do this and waste a long time)
//
// So: only call boot_putk right after you have completely 
// received a message and the laptop side is quietly waiting 
// for a response.
static inline void boot_putk(const char *msg) {
    // send the <PRINT_STRING> opcode
    boot_put32(PRINT_STRING);

    unsigned n = strlen(msg);
    // send length.
    boot_put32(strlen(msg));

    // send the bytes in the string [we don't include 0]
    for(n = 0; msg[n]; n++)
        uart_put8(msg[n]);
}

// example of how to use macros to get file and lineno info
// if we don't do the LINE_STR() hack (see assert.h), 
// what happens?
#define boot_todo(msg) \
   boot_err(BOOT_ERROR, __FILE__ ":" LINE_STR() ":TODO:" msg "\n")

// send back an error and die.   note: we have to flush the output
// otherwise rebooting could trash the TX queue (example of a hardware
// race condition since both reboot and TX share hardware state)
static inline void 
boot_err(uint32_t error_opcode, const char *msg) {
    boot_putk(msg);
    boot_put32(error_opcode);
    uart_flush_tx();
    rpi_reboot();
}

#endif
