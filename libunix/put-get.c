#include "libunix.h"

#if 0
// this does not work on macos.
#include <endian.h>
// this shows how to use <_Static_assert> to check things statically.
_Static_assert(__BYTE_ORDER == __LITTLE_ENDIAN, 
            "code only written for little-endian machines");
#endif

void put_uint8(int fd, uint8_t b)   { write_exact(fd, &b, 1); }
void put_uint32(int fd, uint32_t u) { write_exact(fd, &u, 4); }

uint8_t get_uint8(int fd) {
    uint8_t b;
    read_exact(fd, &b, 1);
    return b;
}

// we do 4 distinct get_uint8's b/c the bytes get dribbled back to 
// use one at a time --- when we try to read 4 bytes at once it can
// (occassionally!) fail.
//
// NOTE: a more general approach: we could make a version of 
// read_exact that will keep trying until a timeout.
//
// note: the other way to do is to assign these to a char array b and 
//  return *(unsigned)b
// however, the compiler doesn't have to align b to what unsigned 
// requires, so this can go awry.  easier to just do the simple way.
// we do with |= to force get_byte to get called in the right order 
//  (get_byte(fd) | get_byte(fd) << 8 ...) 
// isn't guaranteed to be called in that order b/c '|' is not a seq point.
uint32_t get_uint32(int fd) {
    uint32_t u;
    u  = get_uint8(fd);
    u |= get_uint8(fd) << 8;
    u |= get_uint8(fd) << 16;
    u |= get_uint8(fd) << 24;
    return u;
}
