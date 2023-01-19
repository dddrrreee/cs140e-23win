#include "rpi.h"

int rpi_putchar(int c) { uart_putc(c); return c; }
