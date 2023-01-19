#include "rpi.h"

int putk(const char *p) {
    for(; *p; p++)
        rpi_putchar(*p);
//  we were adding this.
//  rpi_putchar('\n');
    return 1;
}
