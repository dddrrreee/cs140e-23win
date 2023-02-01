#include "libunix.h"

// write_exact will trap errors.
void put_uint8(int fd, uint8_t b)   { write_exact(fd, &b, 1); }
void put_uint32(int fd, uint32_t u) { write_exact(fd, &u, 4); }

uint8_t get_uint8(int fd) {
    uint8_t b;

    int res;
    if((res = read(fd, &b, 1)) < 0) 
        die("my-install: tty-USB read() returned error=%d: disconnected?\n", res);
    if(res == 0)
        die("my-install: tty-USB read() returned 0 bytes.  r/pi not responding [reboot it?]\n");

    // impossible for anything else.
    assert(res == 1);
    return b;
}

// we do 4 distinct get_uint8's b/c the bytes get dribbled back to 
// use one at a time --- when we try to read 4 bytes at once it will
// fail.
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
