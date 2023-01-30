#ifndef __LIBUNIX_H__
#define __LIBUNIX_H__
// engler: put all the prototypes you define in this header file.
#include <assert.h>
#include <stdlib.h>

// staff-provided prototypes and routines.
#include "staff-libunix.h"

//
// You need to implement:
//      - find_ttyusb* (in find-ttyusb.c)
//      - read_file  (in read-file.c)
//


// looks in /dev for a ttyusb device. 
// returns:
//  - device name.
// panic's if 0 or more than 1.
char *find_ttyusb(void);
char *find_ttyusb_first(void);
char *find_ttyusb_last(void);

// read in file <name>
// returns:
//  - pointer to the code.  pad code with 0s up to the
//    next multiple of 4.  
//  - bytes of code in <size>
//
// fatal error open/read of <name> fails.
void *read_file(unsigned *size, const char *name);


// create file <name>: truncates if already exists.
int create_file(const char *name);
FILE *fcreate_file(const char *name);

// if you want bit-manipulation routines.
#include "bit-support.h"

// uncomment if you want time macros
// #include "time-macros.h"


// add any other prototypes you want!


// waits for <usec>
int can_read_timeout(int fd, unsigned usec);
// doesn't block.
int can_read(int fd);

int read_timeout(int fd, void *data, unsigned n, unsigned timeout);


// print argv style string.
void argv_print(const char *msg, char *argv[]);


// roundup <x> to a multiple of <n>: taken from the lcc compiler.
#define pi_roundup(x,n) (((x)+((n)-1))&(~((n)-1)))

// non-blocking check if <pid> exited cleanly.
// returns:
//   - 0 if not exited;
//   - 1 if exited cleanly (exitcode in <status>, 
//   - -1 if exited with a crash (status holds reason)
int child_clean_exit_noblk(int pid, int *status);
// blocking version.
int child_clean_exit(int pid, int *status);


// return current number of usec --- probably better to make a larger datatype.
// makes printing kinda annoying however.
// this should be u64
typedef unsigned time_usec_t;
time_usec_t time_get_usec(void);
unsigned time_get_sec(void);

// <fd> is open?  return 1, else 0.
int is_fd_open(int fd);

// given a fd in the current process <our_fd>,
// fork/exec <argv> and dup it to <child_fd>
void handoff_to(int our_fd, int child_fd, char *argv[]);

// close all open file descriptors except 0,1,2 and <fd>
void close_open_fds_except(int fd);
// close all open file descriptors except 0,1,2.
void close_open_fds(void);

#include "fast-hash32.h"

// look for a pi binary in "./" or colon-seperated list in
// <PI_PATH> 
const char *find_pi_binary(const char *name);

#endif

