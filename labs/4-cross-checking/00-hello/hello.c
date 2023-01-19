#include "rpi.h"
#include "foo.h"

void notmain(void) {
    printk("hello -- about to call foo()\n");
    foo();
    clean_reboot();
}
