#include "rpi.h"

void notmain(void) {
    volatile unsigned *null_ptr = 0;

    printk("about to write to null [%p]\n", null_ptr);
    *null_ptr = 1;
    printk("null write succeeded??   result = %d\n", *null_ptr);
}
