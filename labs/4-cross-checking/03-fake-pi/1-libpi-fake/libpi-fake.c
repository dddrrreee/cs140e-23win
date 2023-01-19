#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include "rpi.h"

int printk(const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
        int res = vprintf(fmt, ap);
    va_end(ap);

    return res;
}

void clean_reboot(void) {
    printk("DONE!!!\n");
    exit(0);
}

// this only works because we are compiling a single routine 
// otherwise we'll get multiple definition errors.
int main(void) {
    notmain();
    return 0;
}
