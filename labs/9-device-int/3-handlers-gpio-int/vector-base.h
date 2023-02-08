#ifndef __VECTOR_BASE_SET_H__
#define __VECTOR_BASE_SET_H__
#include "libc/bit-support.h"
#include "asm-helpers.h"

/*
 * vector base address register:
 *   3-121 --- let's us control where the exception jump table is!
 *
 * defines: 
 *  - vector_base_set  
 *  - vector_base_get
 */

cp_asm_set(vector_base_asm, p15, 0, c12, c0, 0)
cp_asm_get(vector_base_asm, p15, 0, c12, c0, 0)

static inline void *vector_base_get(void) {
    return (void*)vector_base_asm_get();
}

// check alignment.
static inline int vector_base_chk(void *vector_base) {
    if(!vector_base) {
        // output("null vector base!\n");
        return 0;
    }

    uint32_t v = (uint32_t)vector_base;
    if(bits_get(v, 0, 4) != 0) {
        // output("lower 5 bits of the vector base should be 0, have: %x\n", v);
        return 0;
    }
    
    return 1;
}

static inline void vector_base_set(void *vec) {
    if(!vector_base_chk(vec))
        panic("illegal vector base %p\n", vec);

    void *v = vector_base_get();
    if(v) 
        panic("vector base register already set=%p\n", v);
    vector_base_asm_set((uint32_t)vec);

    v = vector_base_get();
    if(v != vec)
        panic("set vector=%p, but have %p\n", vec, v);
}

static inline void *
vector_base_reset(void *vector_base) {
    if(!vector_base_chk(vector_base))
        panic("illegal vector base %p\n", vector_base);

    void *old_vec = vector_base_get();
    vector_base_asm_set((uint32_t)vector_base);
    assert(vector_base_get() == vector_base);

    return old_vec;
}
#endif
