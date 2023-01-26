#include "rpi.h"

int recurse(int n) {
    if(n <= 0)
        return 0;
    return 1 + recurse(n-1);
}

void notmain(void) {
    printk("hello: recurse=%d\n", recurse(10));
}
