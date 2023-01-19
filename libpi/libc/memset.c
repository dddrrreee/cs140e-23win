#include "rpi.h"

#define aligned(ptr, n)  ((unsigned)ptr % n == 0)
#define aligned4(ptr)  aligned(ptr,4)
#define aligned8(ptr)  aligned(ptr,8)

void *memset(void *dst, int c, size_t n) {
    assert(n);

    if(!c) {
        if(aligned8(dst) && aligned8(n)) {
            uint64_t *p = dst;
            n = n / 8;
            while(n-- > 0)
                *p++ = c;
            return p;
        } else if(aligned4(dst) && aligned4(n)) {

            uint32_t *p = dst;
            n = n / 4;
            while(n-- > 0)
                *p++ = c;
            return p;
        }
    }

    char *p = dst, *e = p + n;
    while(p < e)
        *p++ = c;
    return p;
}

#ifdef memset
    void *(memset)(void *dst, int c, size_t n) {
        return memset(dst,c,n);
    }
#endif
