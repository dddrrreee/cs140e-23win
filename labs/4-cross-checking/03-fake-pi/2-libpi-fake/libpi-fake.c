#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include "rpi.h"

void uart_putc(unsigned c) {
    putchar(c);
}
void uart_flush_tx(void) {
    fflush(stdout);
}

//  printk comes from libpi
//  putk comes from libpi
//  rpi_putchar comes from libpi


// just ignore
void delay_ms(unsigned ms) {
}
void rpi_reboot(void) { 
    exit(0);
}

#if 0
void clean_reboot(void) {
    printk("DONE!!!\n");
    exit(0);
}
#endif

// this only works because we are compiling a single routine 
// otherwise we'll get multiple definition errors.
int main(void) {
    notmain();
    return 0;
}
