#include "rpi.h"

void notmain(void) {
    printk("hello world\n");
    clean_reboot();
}
