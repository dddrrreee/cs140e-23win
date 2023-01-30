#ifndef __FAST_HASH32_H__
#define __FAST_HASH32_H__

// http://www.azillionmonkeys.com/qed/hash.html
#include <stdint.h>

#undef get16bits
#if (defined(__GNUC__) && defined(__i386__)) || defined(__WATCOMC__) \
  || defined(_MSC_VER) || defined (__BORLANDC__) || defined (__TURBOC__)
#define get16bits(d) (*((const uint16_t *) (d)))
#endif

#if !defined (get16bits)
#define get16bits(d) ((((uint32_t)(((const uint8_t *)(d))[1])) << 8)\
                       +(uint32_t)(((const uint8_t *)(d))[0]) )
#endif

static inline uint32_t fast_hash_inc32(const void * _data, uint32_t len, uint32_t hash) {
    const char * data = (const void*)_data;
    uint32_t tmp;
//    assert(len % 4 == 0 && data);
    len >>= 2;

    /* Main loop */
    for (;len > 0; len--) {
        hash  += get16bits (data);
        tmp    = (get16bits (data+2) << 11) ^ hash;
        hash   = (hash << 16) ^ tmp;
        data  += 2*sizeof (uint16_t);
        hash  += hash >> 11;
    }

    /* Force "avalanching" of final 127 bits */
    hash ^= hash << 3;
    hash += hash >> 5;
    hash ^= hash << 4;
    hash += hash >> 17;
    hash ^= hash << 25;
    hash += hash >> 6;

    return hash;
}

// put your extra libc prototypes here.
uint32_t fast_hash(const void *data, uint32_t len);
uint32_t fast_hash_inc(const void * _data, uint32_t len, uint32_t hash);

#endif
