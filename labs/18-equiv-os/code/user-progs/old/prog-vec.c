#include "libos.h"

enum { N  = 16 };

void notmain(void) {
    volatile unsigned *v = sys_sbrk(0);

    output("sbrk gives: %p for $pid\n", v);

    for(int i = 0; i < N; i++)
        v[i] = i;

    unsigned prod = 1;
    for(int i = 0; i < N; i++)
        prod *= v[i];

    output("prod=%x for $pid\n", prod);
    sys_exit(prod);
}
