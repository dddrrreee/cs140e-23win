#ifndef __LIBUNIX_STAFF_H__
#define __LIBUNIX_STAFF_H__
// engler, cs140e/cs240lx
// 
// these are the staff provided routines.
//    DO NOT MODIFY!!!  Add your prototypes to libunix-extra.h
//    DO NOT MODIFY!!!  Add your prototypes to libunix-extra.h
//    DO NOT MODIFY!!!  Add your prototypes to libunix-extra.h
//    DO NOT MODIFY!!!  Add your prototypes to libunix-extra.h
//    DO NOT MODIFY!!!  Add your prototypes to libunix-extra.h
//    DO NOT MODIFY!!!  Add your prototypes to libunix-extra.h
//
// prototypes for different useful unix utilities.  we also mix in
// some pi-specific unix-side routines since it's easier to keep them
// in one place.
//
#include <stdint.h>
#include <sys/types.h>
#include <unistd.h>

// roundup <x> to a multiple of <n>: taken from the lcc compiler.
#define pi_roundup(x,n) (((x)+((n)-1))&(~((n)-1)))

// bunch of useful macros for debugging/error checking.
#include "demand.h"

// read and echo the characters from the pi's usbtty to unix
// and from unix to pi until the ttyusb disappears (the hardware got
// pulled out) or we see a <DONE!!!> string, used to indicate 
// a clean shutdown.
void pi_cat(int pi_fd, const char *portname);

// hack-y state machine to indicate when we've seen the special string
// 'DONE!!!' from the pi telling us to shutdown.  pass in the null
// terminated string read from the pi.
int pi_done(unsigned char *s);

// overwrite any unprintable characters with a space.
// otherwise terminals can go haywire/bizarro.
// note, the string can contain 0's, so we send the
// size.
void remove_nonprint(uint8_t *buf, int n);
#define HANDOFF_FD 21

// same as above, but returns 0 if can't open.
void *read_file_canfail(unsigned *size, const char *name);

// read file, expect it to be <size> bytes.
int read_file_noalloc(const char *name, void *buf, unsigned maxsize);

// opens the ttyusb <device> and returns file descriptor.
int open_tty(const char *device);
int open_tty_n(const char *device, int maxattempts);

// used to set a tty to the 8n1 protocol needed by the tty-serial.
int set_tty_to_8n1(int fd, unsigned speed, double timeout);

// returns 1 if the tty is not there, 0 otherwise.
// used to detect when the user pulls the tty-serial device out.
int tty_gone(const char *ttyname);

// return current number of usec --- probably better to make a larger datatype.
// makes printing kinda annoying however.

// call this to check errors for closing a descriptor:
#define close_nofail(fd) no_fail(close(fd))


uint32_t our_crc32(const void *buf, unsigned size);
// our_crc32_inc(buf,size,0) is the same as our_crc32 
uint32_t our_crc32_inc(const void *buf, unsigned size, uint32_t crc);

// fill in <fmt,..> using <...> and strcat it to <dst>
char *strcatf(char *dst, const char *fmt, ...);

// return a strdup's string.
char *strdupf(const char *fmt, ...);
char *strcpyf(char *dst, const char *fmt, ...);
char *vstrdupf(const char *fmt, va_list ap);
// concat src1 with the result of fmt... and return strdup'd result
char *str2dupf(const char *src1, const char *fmt, ...);

// write exactly <n> bytes: panics if short write.
int write_exact(int fd, const void *data, unsigned n);
// read exactly <n> bytes: panics if short read.
int read_exact(int fd, void *data, unsigned n);

void put_uint8(int fd, uint8_t b);
void put_uint32(int fd, uint32_t u);
uint8_t get_uint8(int fd);
uint32_t get_uint32(int fd);


#if 0
// this is used for lab 5 and onward.
enum { TRACE_FD = 21 };
#endif


int suffix_cmp(const char *s, const char *suffix);
int prefix_cmp(const char *s, const char *prefix);

void run_system(const char *fmt, ...);
int run_system_err_ok(int verbose_p, const char *fmt, ...) ;


// lookup <name> in directory <path> and return <path>/<name>
char *name_lookup(const char *path, const char *name);

void pi_echo(int unix_fd, int pi_fd, const char *portname);

int exists(const char *name);

#endif
