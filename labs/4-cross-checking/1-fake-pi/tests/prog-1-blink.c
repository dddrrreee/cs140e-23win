// part 1: uses your GPIO code to blink a single LED connected to 
// pin 20.
//   - when run should blink 10 times.  
//   - you will have to restart the pi by pulling the usb connection out.
#include "rpi.h"

void notmain(void) {
    int led = 20;
    gpio_set_output(led);
    for(int i = 0; i < 10; i++) {
        gpio_set_on(led);
        delay_cycles(3000000);
        gpio_set_off(led);
        delay_cycles(3000000);
    }
}
