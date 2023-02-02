// low level print that makes a bit easier to debug.
#include "rpi.h"

static void my_putk(char *s) {
    for(; *s; s++)
        uart_put8(*s);
}

void notmain(void) {
    uart_init();
    my_putk("TRACE:hello world using my_putk\n");
    printk("TRACE:hello world using printk\n");
}
