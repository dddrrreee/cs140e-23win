#include "rpi.h"

#if 0
int rpi_putchar(int c) { 
    uart_put8(c); 
    return c; 
}
#endif
typedef int (*rpi_putchar_t)(int chr);

static int default_putchar(int c) { 
    uart_put8(c); 
    return c; 
}

rpi_putchar_t rpi_putchar = default_putchar;

void rpi_putchar_set(rpi_putchar_t putc) {
    rpi_putchar = putc;
}
