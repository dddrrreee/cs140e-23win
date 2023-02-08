#include "rpi.h"

void notmain(void) {
    int pin = 20;
    trace("event detected = %d\n", gpio_event_detected(pin));
    trace("event detected = %d\n", gpio_event_detected(pin));
    trace("event detected = %d\n", gpio_event_detected(pin));
    trace("event detected = %d\n", gpio_event_detected(pin));
    trace("event detected = %d\n", gpio_event_detected(pin));
}
