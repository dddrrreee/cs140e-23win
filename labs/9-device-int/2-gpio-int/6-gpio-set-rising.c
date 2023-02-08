#include "rpi.h"

void notmain(void) {
    int pin = 20;
    gpio_int_rising_edge(pin);
}
