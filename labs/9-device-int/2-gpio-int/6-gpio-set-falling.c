#include "rpi.h"

void notmain(void) {
    int pin = 20;
    gpio_int_falling_edge(pin);
}
