#include "rpi.h"

void notmain(void) {
    printk("hello: things worked!\n");
    clean_reboot();
}
