#include "rpi.h"

void my_putk(const char *msg) {
    while(*msg)
        uart_put8(*msg++);
}

void notmain(void) {
    uart_init();
    my_putk("hello\n");
}
