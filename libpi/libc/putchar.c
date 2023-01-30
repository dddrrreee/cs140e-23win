#include "rpi.h"

int rpi_putchar(int c) { 
    uart_put8(c); 
    return c; 
}
