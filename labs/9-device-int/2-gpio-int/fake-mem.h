#ifndef __FAKE_MEM_H__
#define __FAKE_MEM_H__

// trivial memory
#include <assert.h>
#include <stdint.h>

typedef struct {
    uint32_t addr;
    uint32_t val;
} mem_ent_t;

// is there even any reason to do it like this?
#define MEM_MAX 128
typedef struct {
    unsigned n;
    mem_ent_t mem[MEM_MAX];
} mem_t;

static inline mem_t mem_mk(void) {
    return (mem_t) {};
}

static mem_ent_t mem_ent_mk(uint32_t addr, uint32_t val) {
    return (mem_ent_t) { .addr = addr, .val = val };
}

// lookup <addr> in your memory implementation.  Returns the associated <mem_t>
// or 0 if there is none (null).
static mem_ent_t* mem_lookup(mem_t *m, uint32_t addr) {
    for(int i = 0; i < m->n; i++)
        if(m->mem[i].addr == addr)
            return &m->mem[i];
    return 0;
}


// insert (<addr>, <val>) into your fake memory.  
// do an assertion check that:
//   - before: <addr> is not in fake memory.
//   - after: <addr> is in fake memory and lookup returns the corect <val>
static inline mem_ent_t *
mem_insert(mem_t *m, uint32_t addr, uint32_t val) {
    assert(m->n < MEM_MAX);
    assert(!mem_lookup(m, addr));
    mem_ent_t *e = &m->mem[m->n++];
    *e = mem_ent_mk(addr, val);
    return e;
}

#if 0
static void mem_write(mem_t *mem, uint32_t addr, uint32_t val) {
    mem_ent_t *m = mem_lookup(mem,addr);
    if(m)
        m->val = val;
    else
        mem_insert(mem,addr,val);
}
#endif

static inline mem_ent_t *mem_last(mem_t *m) {
    if(!m->n)
        return 0;
    return &m->mem[m->n-1];
}



#ifdef TEST
#include <stdio.h>
int main(void) {
    mem_t m = mem_mk();

    unsigned n = 10;
    for(int i = 0; i < n; i++) 
        mem_insert(&m, i*10, i * i);
    assert(m.n == n);
    for(int i = 0; i < n; i++) {
        mem_ent_t *e = mem_lookup(&m, i*10);
        assert(e);
        assert(e->addr == i*10);
        assert(e->val == i*i);
    }
    printf("passed %d tests\n",n);
    return 0;
}
#endif

#endif
