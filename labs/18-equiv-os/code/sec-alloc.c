#include "rpi.h"
#include "equiv-os.h"

// very dumb freemap: should add reference counts.
// 1 = free.
enum { MAX_SECS = 512 };
static uint8_t sections[MAX_SECS];
static unsigned n_sections;

void sec_alloc_init(unsigned nsec) {
    assert(nsec>0 && nsec < MAX_SECS);
    n_sections = nsec;
    memset(sections, 1, nsec);
}

int sec_is_legal(uint32_t secn) {
    if(secn >= n_sections)
        panic("trying to allocate section %d: max should be %d\n", secn, n_sections);
    return secn < n_sections;
}

void sec_free(uint32_t secn) {
    assert(sec_is_legal(secn));
    if(sections[secn])
        panic("section %d is not allocated!\n");
    sections[secn] = 1;
}

long sec_alloc_exact(uint32_t secn) {
    assert(sec_is_legal(secn));

    if(!sections[secn]) {
        panic("trying to allocate section %d, but already allocated\n", secn);
        return -1;
    }
    output("allocated sec %d\n", secn);
    sections[secn]=0;
    return secn;
}

// need to change to allocate 16mb.
long sec_alloc(void) {
    for(uint32_t i = 0; i < n_sections; i++)
        if(sections[i])
            return sec_alloc_exact(i);

    panic("can't allocate section\n");
    return -1;
}
