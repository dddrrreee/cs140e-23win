#include "rpi.h"

void notmain(void) {
    for(int i = 0; i < 34; i++)
        gpio_int_falling_edge(i);
}
