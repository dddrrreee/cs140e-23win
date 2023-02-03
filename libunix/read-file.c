#include <assert.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

#include "libunix.h"

// allocate buffer, read entire file into it, return it.   
// buffer is zero padded to a multiple of 4.
//
//  - <size> = exact nbytes of file.
//  - for allocation: round up allocated size to 4-byte multiple, pad
//    buffer with 0s. 
//
// fatal error: open/read of <name> fails.
//   - make sure to check all system calls for errors.
//   - make sure to close the file descriptor (this will
//     matter for later labs).
// 
void *read_file(unsigned *size, const char *name) {
    int fd;
    struct stat st;
    assert(name != NULL);

    fd = open(name, O_RDONLY);
    if (fd == -1) {
        perror("open");
        return NULL;
    }

    if (fstat(fd, &st) == -1) {
        perror("fstat");
        return NULL;
    }

    *size = st.st_size;

    unsigned padded_size = (*size + 3) & ~0x03;
    char *buffer = calloc(1, padded_size);
    if (!buffer) {
        perror("calloc");
        return NULL;
    }

    ssize_t read_size = read(fd, buffer, *size);
    if (read_size == -1) {
        perror("read");
        return NULL;
    }

    if (close(fd) == -1) {
        perror("close");
        return NULL;
    }

    return buffer;
    // How: 
    //    - use stat to get the size of the file.
    //    - round up to a multiple of 4.
    //    - allocate a buffer
    //    - zero pads to a multiple of 4.
    //    - read entire file into buffer.  
    //    - make sure any padding bytes have zeros.
    //    - return it.   
    //unimplemented();
}
