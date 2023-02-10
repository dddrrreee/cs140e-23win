#include "rpi.h"

void notmain(void) {
    int pin = 20;
    gpio_event_clear(pin);
}
