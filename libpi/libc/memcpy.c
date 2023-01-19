#include "rpi.h"

#define aligned(ptr, n)  ((unsigned)ptr % n == 0)
#define aligned4(ptr)  aligned(ptr,4)
#define aligned8(ptr)  aligned(ptr,8)

void *memcpy(void *dst, const void *src, size_t nbytes) { 
#if 0
    if(aligned8(dst) && aligned8(src) && aligned8(nbytes)) {
        uint64_t *d = dst;
        const uint64_t *s = src;
        unsigned n = nbytes / 8;
        
        for(unsigned i = 0; i < n; i++)
            d[i] = s[i];


    // this is not for optimization.   when gcc copies structs it may
    // call memcpy.   if the dst struct is a pointer to hw, and we
    // do byte stores, i don't think this will necessarily lead to 
    // good behavior.
    } else if(aligned4(dst) && aligned4(src) && aligned4(nbytes)) {
        unsigned n = nbytes / 4;
        unsigned *d = dst;
        const unsigned *s = src;

        for(unsigned i = 0; i < n; i++)
            d[i] = s[i];
    } else 
#endif
    {
        unsigned char *d = dst;
        const unsigned char *s = src;
        for(unsigned i = 0; i < nbytes; i++)
            d[i] = s[i];
    }
    return dst;
}

// used to get the end of memcpy for backtraces.
void memcpy_end(void) { }
