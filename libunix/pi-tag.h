#ifndef __PITAG_H__
#define __PITAG_H__
// Each appeneded TAG holds:
//  BIN_TAG
//  total nbytes: strlen(string) + 1 + nbytes(B.bin)
//  0 terminated string
//  the binary

#define BIN_TAG 0x12345678

typedef struct {
    int fd;
    unsigned tot_nbytes;      // total size so far.
} pitag_t;

// call this first
pitag_t pitag_start(const char *out, const void *prog, unsigned nbytes);
pitag_t pitag_start_file(const char *out, const char *prog_name);

// append a binary file
void pitag_add_bin(pitag_t *h, const char *bin_name, const void *bin, unsigned bin_nbytes);
void pitag_add_bin_file(pitag_t *h, const char *bin_name);

// call when done.
void pitag_done(pitag_t *h);

#endif
