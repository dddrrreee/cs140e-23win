// the minimal source code for our libos.  could stick 
// in the header, but it wouldn't work if our user 
// processes had more than one file.
#include "libos.h"

// trivial bump allocator: no free.
static char *heap, *heap_end;
static unsigned incr = 1024;
void *talloc(unsigned n) {
    if((heap+n) >= heap_end) {
        if(incr < n)
            incr = n;

        incr *= 2;  // double each time.

        // we assume the heap keeps extending contiguously.
        void *p = sys_sbrk(incr);
        if(!heap)
            heap = p;
        heap_end = heap+incr;
    }
    void *mem = heap;
    heap += n;
    memset(mem, 0, n);
    return mem;
}
