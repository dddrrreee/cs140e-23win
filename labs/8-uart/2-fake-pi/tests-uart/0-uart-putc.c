#include "rpi.h"

// this doesn't actually work b/c no uart_init
void notmain(void) {
    //uart_init();
	uart_put8('h');
}
