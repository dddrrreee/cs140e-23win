#include "libos.h"

enum { N  = 16 };

void notmain(void) {
    unsigned *v = talloc(N*4);
    output("sbrk gives: %x\n", v);

    for(int i = 0; i < N; i++)
        v[i] = i;

    // this detects misalignment issues, among other things.
    for(int i = 0; i < N; i++) {
        if(v[i] != i)
            panic("not equal??\n");
        output("v[%d]=%d\n", i,v[i]);
    }

    unsigned prod = 1;
    for(int i = 0; i < N; i++) {
        output("prod=%d: v[%d]=%d\n", prod,i,v[i]);
        prod *= (v[i]+1);
    }

    output("prod=%x\n", prod);
    assert(prod == 0x77758000);
    sys_exit(prod);
}
