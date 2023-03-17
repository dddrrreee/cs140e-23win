#include "libos.h"

enum { N  = 1 };

void notmain(void) {
    volatile unsigned *v = talloc(N*4);
    for(int i = 0; i < N; i++)
        v[i] = i;
    sys_exit(0);
}
