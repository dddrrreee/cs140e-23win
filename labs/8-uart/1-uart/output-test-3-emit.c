// low level print that makes a bit easier to debug.
#include "rpi.h"

void notmain(void) {
    extern void uart_destroy(void);
    
    uart_destroy();
    uart_init();

    // make sure the queues are working.
    
    for(unsigned i = 0; i < 64; i++) {
        printk("TRACE:");
        for(unsigned l = 0; l < 26; l++)  {
            uart_put8(l + 'a');
            uart_put8(l + 'A');
        }

        uart_put8('\n');
    }
    // oh: wow: nasty bug: if you don't flush tx, this breaks.
    // uart_init();
}
