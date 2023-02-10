#ifndef __PI_BITSUPPORT_H__
#define __PI_BITSUPPORT_H__
// some simple bit manipulation functions: helps make code clearer.

// set x[bit]=0 (leave the rest unaltered) and return the value
static inline uint32_t 
bit_clr(uint32_t x, unsigned bit) {
    assert(bit<32);
    return x & ~(1<<bit);
}

// set x[bit]=1 (leave the rest unaltered) and return the value
static inline uint32_t 
bit_set(uint32_t x, unsigned bit) {
    assert(bit<32);
    return x | (1<<bit);
}

static inline uint32_t 
bit_not(uint32_t x, unsigned bit) {
    assert(bit<32);
    return x ^ (1<<bit);
}

#define bit_isset bit_is_on
#define bit_get bit_is_on

// is x[bit] == 1?
static inline unsigned 
bit_is_on(uint32_t x, unsigned bit) {
    assert(bit<32);
    return (x >> bit) & 1;
}
static inline unsigned 
bit_is_off(uint32_t x, unsigned bit) {
    return bit_is_on(x,bit) == 0;
}

// return a mask with the <n> low bits set to 1.
//  error if nbits > 32.  ok if nbits = 0.
//
// we use this routine b/c unsigned shift by a variable greater than 
// bit-width gives unexpected results.  
// eg. gcc on x86:
//  n = 32;
//  ~0 >> n == ~0
static inline uint32_t bits_mask(unsigned nbits) {
    // all bits on.
    if(nbits==32)
        return ~0;
    assert(nbits >= 0 && nbits<32);
    return (1 << nbits) - 1;
}

// extract bits [lb:ub]  (inclusive)
static inline uint32_t 
bits_get(uint32_t x, unsigned lb, unsigned ub) {
    assert(lb <= ub);
    assert(ub < 32);
    return (x >> lb) & bits_mask(ub-lb+1);
}

// set bits[off:n]=0, leave the rest unchanged.
static inline uint32_t 
bits_clr(uint32_t x, unsigned lb, unsigned ub) {
    assert(lb <= ub);
    assert(ub < 32);

    uint32_t mask = bits_mask(ub-lb+1);

    // XXX: check that gcc handles shift by more bit-width as expected.
    return x & ~(mask << lb);
}

// set bits [lb:ub] to v (inclusive).  v must fit within the specified width.
static inline uint32_t 
bits_set(uint32_t x, unsigned lb, unsigned ub, uint32_t v) {
    assert(lb <= ub);
    assert(ub < 32);

#if 0
    // XXX: not sure if special casing is clearer, or if we just keep the
    if(lb == 0 && ub == 31)
        return v;
#endif
    unsigned n = ub-lb+1;
    assert(n<=32);
    assert((bits_mask(n) & v) == v);

    return bits_clr(x, lb, ub) | (v << lb);
}

// bits [lb:ub] == <val?
static inline unsigned 
bits_eq(uint32_t x, unsigned lb, unsigned ub, uint32_t val) {
    assert(lb <= ub);
    assert(ub < 32);
    return bits_get(x, lb, ub) == val;
}

static inline unsigned 
bit_count(uint32_t x) {
    unsigned cnt = 0;
    for(unsigned i = 0; i < 32; i++)
        if(bit_is_on(x,i))
            cnt++;
    return cnt;
}

static inline uint32_t 
bits_union(uint32_t x, uint32_t y) {
    return x | y;
}

static inline uint32_t 
bits_intersect(uint32_t x, uint32_t y) {
    return x & y;
}
static inline uint32_t bits_not(uint32_t x) {
    return ~x;
}

// forall x in A and not in B
static inline uint32_t 
bits_diff(uint32_t A, uint32_t B) {
    return bits_intersect(A, bits_not(B));
}

#endif
