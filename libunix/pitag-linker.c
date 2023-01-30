// append various records to the end of a pi binary.   atm doesn't do anything besides
// programs.
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "libunix.h"
#include "pi-tag.h"

#if 1
#define link_debug(args...) do { } while(0)
#else
#define link_debug(args...) debug(args)
#endif

#if 0
#define BIN_TAG 0x12345678

typedef struct {
    int fd;
    unsigned tot_nbytes;      // total size so far.
} pitag_t;
#endif

pitag_t pitag_start(const char *out, const void *prog, unsigned nbytes) {
    int fd = create_file(out);
    assert(fd>0);

    write_exact(fd, prog, nbytes);
    return (pitag_t){ .fd = fd, .tot_nbytes = nbytes };
}
pitag_t pitag_start_file(const char *out, const char *prog_name) {
    unsigned nbytes;
    uint32_t *prog = (void*)read_file(&nbytes, prog_name);
    return pitag_start(out, prog, nbytes);
}

// push a single record.
//  - <bin_name> name associate with the code.
//  - <bin> = binary (executable) code.
//  - <nbytes> size of <bin> in bytes
void pitag_add_bin(pitag_t *h, const char *bin_name, const void *bin, unsigned bin_nbytes) {
    // word 0
    unsigned tag = BIN_TAG;
    write_exact(h->fd, &tag, sizeof tag);

    // word 1: the total number of bytes in this log record.
    //   you can skip ahead this much to get to the next record.
    unsigned entry_nbytes = strlen(bin_name) + 1 + bin_nbytes + 4*4;
    link_debug("total bytes in log record should be: %d\n", entry_nbytes);
    write_exact(h->fd, &entry_nbytes, sizeof entry_nbytes);

    // word 2: number of bytes in binary
    link_debug("B bytes should be: %d\n", bin_nbytes);
    write_exact(h->fd, &bin_nbytes, sizeof bin_nbytes);

    // word 3: expected offset in the enclosing file: should move this up.
    link_debug("offset should be: %d\n", h->tot_nbytes);
    write_exact(h->fd, &h->tot_nbytes, sizeof h->tot_nbytes);

    // write the binary out.
    write_exact(h->fd, bin, bin_nbytes);

    link_debug("string should be: <%s>\n", bin_name);
    write_exact(h->fd, bin_name, strlen(bin_name)+1);

    // where the next entry will start.
    h->tot_nbytes += entry_nbytes;
}

void pitag_add_bin_file(pitag_t *h, const char *bin_name) {
    unsigned nbytes;
    uint32_t *bin = (void*)read_file(&nbytes, bin_name);
    pitag_add_bin(h, bin_name, bin, nbytes);
}

void pitag_done(pitag_t *h) {
    close(h->fd);
}

