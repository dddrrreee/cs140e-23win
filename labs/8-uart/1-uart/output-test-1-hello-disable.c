// low level print that makes a bit easier to debug.
#include "rpi.h"

static void my_putk(char *s) {
    for(; *s; s++)
        uart_put8(*s);
}

void notmain(void) {
    for(unsigned i = 0; i < 10; i++) {
        uart_init();
        printk("TRACE:hello world\n");
        uart_disable();
    }
    uart_init();
}
