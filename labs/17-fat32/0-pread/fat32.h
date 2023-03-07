#include <string.h>

// uncomment to turn off the associated debug output.  
// kernels often use levels, but this winds up taking
// a bit less code.
#define debug_lo(msg...)     do { } while(0)
#define debug_med(msg...)    do { } while(0)
// #define debug_hi(msg...)     do { } while(0)

#ifndef debug_lo
#   define debug_lo(msg...)  debug(msg);
#endif
#ifndef debug_med
#   define debug_med(msg...)  debug(msg);
#endif
#ifndef debug_hi
#   define debug_hi(msg...)  debug(msg);
#endif

// opaque type for fat32 file system (see pread-test.c)
typedef struct fat32 fat32_t;

// opaque type for fat32 file (see pread-test.c)
typedef struct fat32_file fat32_file_t;

/**********************************************************
 * FAT32 file system methods (will mirror lab).
 */

// given block (cluster) <blk> get the next one, or 0 if none.
uint32_t fat32_blk_next(const fat32_t *fs, uint32_t blk);

// number of bytes per block in file system <fs>.
uint32_t fat32_nbytes_per_blk(const fat32_t *fs);

/**********************************************************
 * FAT32 file methods (will mirror lab).
 */

// the FAT32 file system that <f> comes from.
fat32_t *file32_fs(const fat32_file_t *f);

// starting block number of file <f> (can be used as an index
// into the FAT table).
uint32_t file32_start(const fat32_file_t *f);

// size of file <f> in bytes.
uint32_t file32_nbytes(const fat32_file_t *f);

// given an offset <off> in file <f> return the containing
// block (or 0 if none).
uint32_t file32_off_to_blkno(const fat32_file_t *f, uint32_t off);

// a pointer to the data associated with data block <blk>
// the size of this block is <fat32_nbytes_per_blk(fs)> bytes.
const void *fat32_blk_pointer(const fat32_t *fs, uint32_t blk);

// implement this: is in fat32-pread.c along with a description.
int fat32_pread(fat32_file_t *f, void *data, uint32_t nbytes, uint32_t off);
