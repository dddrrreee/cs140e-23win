#include "libunix.h"
#include "fat32.h"

// <fat32_pread>:
//    copies at most <nbytes> of data from file <f> into buffer <data> 
//    starting at file offset <off>.  
//
//    truncates requests that would go beyond the end of the file.
//
//    note: input comes from the user (evil to careless) so you must do
//    sanity checking and ensure <fat32_pread> works even if <nbytes> 
//    or <off> would cause arithemetic overflow.
//
//    returns 0 and does not write to <data> if:
//     - <nbytes> is zero 0.
//     - offset <off> is larger-than/equal-to the file size.
//
//    otherwise, returns the number of bytes copied.  (if no truncation 
//    occurs, this value will be <nbytes>)
//
// <pread> is a short routine that is tricky to write.
//
// A bunch of file system irritation is just dealing with the fact
// that  you have to copy out / copy in a contiguous array onto a 
// set of non-contiguous file blocks.  
//
// Its annoying to test (as you can see from the testing code!  OMG
// I can't believe how long it is).
//
// ******************************************************************
// To implement, you should use (see fat32.h and tests-pread.c):
//    - file32_off_to_blkno(f, off) to get the file block (cluster)
//      to start reading from, given <f> and <off>
//
//    - fat32_blk_next(fs,blk): to iterate over the blocks in the file.
// 
//    - given a block <blk> call <fat32_blk_pointer(fs, blk)> to get
//      a pointer to the start of the data block.
//
// To implement:
//  1. I would start first by making sure you can copy out an entire contiguous
//     file from start to finish (the test should do this)
//  2. Then make it work with copying a byte.
//  3. Then the rest.
//  4. Use your debugger!  and also different debug statements if you
//     get stuck.   a useful feature of gdb or lldb is that you can
//     get a backtrace (gdb: "bt") to see the callstack above you 
//     if there is a crash.
int fat32_pread(fat32_file_t *f, void *data, uint32_t nbytes, uint32_t off) {
    assert(f);
    uint32_t file_nbytes = file32_nbytes(f);

    if(!file_nbytes)
        return 0;

    if(off >= file_nbytes)
        return 0;

    fat32_t *fs = file32_fs(f);
    uint32_t nbytes_per_blk = fat32_nbytes_per_blk(fs);

    todo("implement the rest of this code\n");

    return nbytes;
}
