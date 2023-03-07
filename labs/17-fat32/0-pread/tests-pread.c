/*
 * a bunch of testing infrastructure to try to check that your pread
 * works.
 *
 * the infrastructure is a simplified FAT system, so it's useful to 
 * understand as a short dry run for tommorrow.
 *
 *  1. we have a fake disk <fake_disk_t> that has <NBLK> blocks 
 *     of data.
 *
 *     each block is made of <NSEC_PER_BLOCK> sectores (512 bytes).
 *  2. we have a simplified <fat32_t> file system, that contains a 
 *     single fat (each legal entry should point to some unique
 *     block in <fake_disk>.
 *
 *  3. the fat contains a single simplified FAT32 file.  
 *     it has no directories.  fat32 FAT table should only have blocks
 *     that point to the file.
 */
#include "libunix.h"
// #include <string.h>

#include "fat32.h"

enum { 
    // keep these multiple of 2.
    NBLK = 4,                   // number of blocks on fake disk.
    NSEC_PER_BLOCK = 4,         // number of sectors per block.
    SEC_NBYTES = 512,           // probably shouldn't change
    NBYTES_PER_BLOCK = NSEC_PER_BLOCK * SEC_NBYTES
 };

enum { LAST_CLUSTER = 0xfffffff };

/******************************************************************
 * trivial fake disk.  an array of blocks.
 */
typedef struct fake_disk {
    struct {
        uint8_t *data;
    } block[NBLK+2];

    // redundant with the enums.
    unsigned nblks;
    unsigned nsec_per_blk;
    unsigned nbytes_per_blk;
} fake_disk_t;

// make a new fake disk.
static inline fake_disk_t *
disk_new(unsigned nsec_per_blk, unsigned nbytes_per_blk) {
    fake_disk_t *d = calloc(1, sizeof *d);

    d->nblks = NBLK;
    d->nsec_per_blk = nsec_per_blk; 
    d->nbytes_per_blk = nbytes_per_blk;
    for(unsigned i = 2; i < d->nblks; i++)
        d->block[i].data = calloc(1, nbytes_per_blk);
    return d;
}

// return a pointer to disk block <b>
static inline uint8_t *
blk_pointer_mut(fake_disk_t *d, uint32_t b) {
    demand(b < d->nblks, "blk=%d, d->nblks=%d\n", b,d->nblks);
    assert(b>=2);
    assert(d->block[b].data);
    return d->block[b].data;
}
static inline const uint8_t *
blk_pointer(fake_disk_t *d, uint32_t b) {
    return blk_pointer_mut(d,b);
}

// to catch some FAT errors, we only have <nblks>-2 of useful data.
static uint32_t disk_nbytes(fake_disk_t *d) {
    return (d->nblks-2) * d->nbytes_per_blk;
}

// gather all the disk data into a single contig
// array.
static void 
disk_gather(fake_disk_t *d, void *data, unsigned nbytes) {
    assert(disk_nbytes(d) == nbytes);
    
    unsigned n = d->nbytes_per_blk;
    uint8_t *p = data;
    uint8_t *e = p+nbytes;

    debug_lo("nbytes=%d\n", nbytes);
    for(unsigned i = 2; i < d->nblks; i++) {
        debug_lo("about to copy blk=%d [tot=%d]\n", i, d->nblks);
        assert((p+n) <= e);
        memcpy(p, blk_pointer(d, i), n);
        p += n;
    }
}

// randomize <nbytes> of <data>
static void randomize(uint8_t *data, unsigned nbytes) {
    for(unsigned i = 0; i < nbytes; i++)
        data[i] = random();
}


// randomize all the data in the disk.
static void 
disk_randomize(fake_disk_t *d, uint32_t seed) {
    srandom(seed);
    for(unsigned i = 2; i < d->nblks; i++)
        randomize(d->block[i].data, d->nbytes_per_blk);
}

/*****************************************************************
 * trivial fake FAT32 file system.  has a pointer to our fake disk,
 * and a FAT array.
 */

// fake fat32 file system.  has fields you will use in lab,
// though the pointer to the disk is fake.
typedef struct fat32 {
    fake_disk_t *disk;
    unsigned nsec_per_blk;
    unsigned nbytes_per_blk;

    uint32_t (fat)[NBLK];
} fat32_t;

static inline fat32_t 
fat32_mk(unsigned nsec_per_blk) {
    unsigned nbyte = nsec_per_blk * 512;
    fat32_t fs = (fat32_t) { 
            .nsec_per_blk = nsec_per_blk,
            .nbytes_per_blk = nbyte,
            .disk = disk_new(nsec_per_blk, nbyte)
    };

    // initialially in order.
    for(unsigned i = 2; i < NBLK; i++)
        fs.fat[i] = i+1;
    fs.fat[NBLK-1] = LAST_CLUSTER;

    return fs;
}

static inline uint32_t fat_nelem(const fat32_t *fs)
    { return NBLK; }

// given block <b> index into <fs>'s FAT and return the 
// next block, if any.  returns 0 if no more.
//
// note: this is essentially a linked list iteration using
// block pointers, where NULL is replaced with LAST_CLUSTER.
uint32_t fat32_blk_next(const fat32_t *fs, uint32_t b) {
    if(b < 2)
        panic("illegal block=%x [b=%d]\n", b,b);
    if(b >= fs->disk->nblks)
        panic("illegal block=%d\n", b);
    b = fs->fat[b];
    return b == LAST_CLUSTER ? 0 : b;
}

// wrapper to iterate <n> times using fat32_next.
// error if there are not at least <n> FAT more entries.
uint32_t fat32_next_nth(const fat32_t *fs, uint32_t start, uint32_t n) {
    uint32_t b = start;
    for(uint32_t i = 0; i < n; i++) {
        if(!b)
            panic("iterated beyond end of file?  b = %x\n", b);
        b = fat32_blk_next(fs, b);
        debug_lo("next=%d\n", b);
    }
    debug_lo("start=%d, b=%d n=%d\n", start, b,n);
    return b;
}

/*****************************************************************
 * trivial fake FAT32 file.  you iterate over all of it by 
 * using <start> to index into the <fat> array and iterating
 * until you hit LAST_CLUSTER.
 */

// fake fat32 file structure.  has fields you will use in
// lab.
struct fat32_file {
    fat32_t *fs;            // file system file is on.
    unsigned nbytes;
    uint32_t start_blk;     // fat32 == uint32
};

static fat32_file_t 
file32_mk(fat32_t *fs, uint32_t start, unsigned nbytes) {
    return (fat32_file_t) { 
        .fs = fs, 
        .start_blk = start, 
        .nbytes = nbytes 
    };
}

uint32_t fat32_nbytes_per_blk(const fat32_t *fs) {
    return fs->nbytes_per_blk;
}
fat32_t *file32_fs(const fat32_file_t *f) {
    return f->fs;
}
uint32_t file32_nbytes(const fat32_file_t *f) {
    return f->nbytes;
}
const void *fat32_blk_pointer(const fat32_t *fs, uint32_t blk) {
    return blk_pointer(fs->disk, blk);
}

uint32_t file32_off_to_blkno(const fat32_file_t *f, uint32_t off) {
    const fat32_t *fs = f->fs;
    return fat32_next_nth(fs, f->start_blk, off / fs->nbytes_per_blk);
}

/*******************************************************************
 * testing code.
 */

// goal: randomize the fat but do so in such a way
// that we can easily compute the right answer using
// a contiguous reference array.
static void fat_randomize(fat32_t *fs) {
    fake_disk_t *d = fs->disk;
    unsigned nbytes = d->nbytes_per_blk;
    char tmp[nbytes];

    // now go through and randomize order;
    // preserving the elements
    for(unsigned i = 2; i < NBLK; i++) {
        if(fs->fat[i] == LAST_CLUSTER)
            break;

        uint32_t n;
        do {
            n = random() % NBLK;
        } while(n < 2 || fs->fat[n]  == LAST_CLUSTER);

        uint32_t tmp_blk = fs->fat[i];
        uint32_t a_blk   = fs->fat[i] = fs->fat[n];
        uint32_t b_blk   = fs->fat[n] = tmp_blk;

        assert(a_blk >= 2);
        assert(b_blk >= 2);

        // XXX: should just swap pointers.
        void *a = blk_pointer_mut(d, a_blk);
        void *b = blk_pointer_mut(d, b_blk);
        memcpy(tmp, a, nbytes);
        memcpy(a, b, nbytes);
        memcpy(b, a, nbytes);
    }
}

// simple check: read the entire file from start to end.
// this is the most basic test to start with.
static void 
check_read_file(fat32_file_t *f, void *ref_disk, unsigned ref_nbytes) {
    unsigned n = f->nbytes;
    assert(n <= ref_nbytes);

    uint8_t *buf = malloc(n);
    int ret;
    output("about to read entire file from start to finish (nbytes=%d)\n", n);
    if((ret = fat32_pread(f, buf, n, 0)) != n)
        panic("tried to read %d, got  result=%d\n", n, ret);
    assert(memcmp(buf, ref_disk, n) == 0);

    // overflow, sorta
    output("about to read the entire file with overflow\n");
    if((ret = fat32_pread(f, buf, ~0, 0) != n))
        panic("tried to read %d, got %d\n", n, ret);
    assert(memcmp(buf, ref_disk, n) == 0);
    output("success: read entire file\n");
    free(buf);
}

// also simple: read a single byte from the file.
static void check_read_byte(fat32_file_t *f, 
    uint8_t *ref_disk, 
    unsigned ref_nbytes) {

    fat32_t *fs = f->fs;
    unsigned n = f->nbytes;

    // for each byte [0..n) read it from <f>
    for(unsigned i = 0; i < n; i++) {
        uint32_t off = i;

        // use redzones to catch corruptions.
        struct __attribute__((packed)){
            uint64_t rz0;
            uint8_t b;
            uint64_t rz1;
        } v = {0};
        assert(!v.rz0 && !v.rz1);
        debug_med("copying 1 byte at offset %d\n", off);

        // should only get len=1 back.
        int len = fat32_pread(f, &v.b, 1, off);
        if(len != 1)
            panic("should have copied 1 byte, have=%d\n", len);

        // are our redzones still 0?
        if(v.rz0 || v.rz1)
            panic("corrupted redzone, should be 0: rz1=%lx, rz2=%lx\n",
                v.rz0, v.rz1);

        // now look the byte up in the reference array.
        assert(off < ref_nbytes);
        if(ref_disk[off] != v.b)
            panic("expected byte=%x: have=%x\n", ref_disk[off], v.b);
        debug_med("trial=%d: success!\n",i);
    }
    output("success: all n=%d 1 bytes worked!\n", n);
}

// do a bunch of known out of bound reads.  none should succeed.
static void check_out_of_bounds(fat32_file_t *f) { 
    unsigned n = f->nbytes;
    // these should all fail
    output("about to do out of bounds tests to make sure fail\n");

    void *illegal = (void*)1; // should not be mapped

    for(unsigned i = 0; i < n*10; i++) {
        uint32_t off = random();
        uint32_t sz  = random();

        // skip anything not out of bounds
        if(off < n || !sz)
            continue;

        // off should be out of bounds.  sz doesn't matter, other
        // than > 0
        //
        // pass in <illegal> to detect if memory is accessed.
        int len = fat32_pread(f, illegal,  sz, off);
        if(len != 0)
            panic("should have failed: copy is out of file: %d\n", len);

        // do a maximal offset: should fail.
        if(fat32_pread(f, illegal, random()%n, ~0))
            panic("failed\n");
    }

    output("success: all n=%d out of bounds succeeded!!\n", n);
}

// same as above but using u32 --- could be collapsed into
// <check_read_n> but duplicating makes it a bit easier to debug.
static void check_read_u32(fat32_file_t *f, 
    uint8_t *ref_disk, 
    unsigned ref_nbytes) 
{
    fat32_t *fs = f->fs;
    unsigned n = f->nbytes;
    for(unsigned i = 0; i < n; i++) {
        uint32_t off = i;
        struct __attribute__((packed)){
            uint64_t rz0;
            uint32_t got;
            uint64_t rz1;
        } v = {0};
        assert(!v.rz0 && !v.rz1);
        debug_med("copying 4 byte at offset %d\n", off);
        int got_len = fat32_pread(f, &v.got, sizeof v.got, off);

        if(v.rz0 || v.rz1)
            panic("corrupted redzone, should be 0: rz1=%lx, rz2=%lx\n",
                v.rz0, v.rz1);

        unsigned left = n - off;
        unsigned exp_len = 4;
        if(left < exp_len)
            exp_len = left;
        if(exp_len != got_len)
            panic("expected %d nbytes, hvae %d\n", exp_len, got_len);

        uint32_t exp = 0;
        memcpy(&exp, &ref_disk[off], exp_len);

        if(exp != v.got)
            panic("expected byte=%x: have=%x\n", exp, v.got);
        debug_med("trial=%d: success!\n",i);
    }
    output("success: all n=%d 1 bytes worked!\n", n);
    check_out_of_bounds(f);
}

// check that doing a read of <read_nbytes> of data at all different
// offsets works.  also detects that truncated reads are handled (hopefully :).
// could get rid of the previous tests and use just this, but it's harder
// to debug: try to find all your bugs with the previous ones and just
// use this one for "verification".
static void check_read_n(fat32_file_t *f, 
    unsigned read_nbytes,
    uint8_t *ref_disk, 
    unsigned ref_nbytes,
    int verbose_p) 
{
    fat32_t *fs = f->fs;
    unsigned n = f->nbytes;
    uint8_t *buf = malloc(read_nbytes);
    for(unsigned i = 0; i < n; i++) {
        uint32_t off = i;

        debug_med("copying %d=bytes at offset %d\n", read_nbytes, off);
        int got_len = fat32_pread(f, buf, read_nbytes, off);

        assert(off < n);
        unsigned left = n - off;
        unsigned exp_len = read_nbytes;
        if(left < exp_len)
            exp_len = left;
        if(exp_len != got_len)
            panic("expected %d nbytes, hvae %d\n", exp_len, got_len);

        if(memcmp(&ref_disk[off], buf, got_len) != 0)
            panic("mismatch!\n");
        debug_med("trial=%d: success!\n",i);
    }
    free(buf);
    if(verbose_p)
        output("success: all n=%d %d nbytes worked!\n", n, read_nbytes);
}

int main(void)  {
    unsigned sec_nbytes = SEC_NBYTES;
    unsigned nsec_per_blk = NSEC_PER_BLOCK;
    unsigned blk_nbytes = nsec_per_blk * sec_nbytes;

    // make a fake FS
    fat32_t fs = fat32_mk(nsec_per_blk);
    disk_randomize(fs.disk, 0);

    // first do it without randomization.
    unsigned n = disk_nbytes(fs.disk);
    debug_med("allocating %d nbytes\n", n);

    // get a contiguous reference array.
    uint8_t *ref_disk = malloc(n);
    disk_gather(fs.disk,ref_disk, n);

    // start off very simple: file is same size as disk.
    fat32_file_t f = file32_mk(&fs, 2, n);

    // sanity check that the fat is laid out as we expect.
    unsigned nb = fs.disk->nblks-1;
    for(unsigned i = 2; i < nb; i++) {
        uint32_t b = fat32_blk_next(&fs, i);
        if(b != i+1)
            panic("ffiled= b=%d, i+1=%d\n", b, i+1);
        assert(b  == i+1);
    }
    
    output("about to start testing!\n");

    // simple: read the entire file 
    assert(f.nbytes <= n);
    check_read_file(&f, ref_disk, n);

    // simple: read 1 byte at each offset before randomizing fat.
    check_read_byte(&f, ref_disk, n);

    enum { ntrials = 2 };

    unsigned orig = f.nbytes;
    for(int i = 0; i < ntrials; i++) {
        fat_randomize(&fs);
        f.nbytes = orig - (random() % blk_nbytes);
        check_read_file(&f, ref_disk, n);
        f.nbytes = orig - (random() % blk_nbytes);
        check_read_byte(&f, ref_disk, n);
        f.nbytes = orig - (random() % blk_nbytes);
        check_out_of_bounds(&f);
        f.nbytes = orig - (random() % blk_nbytes);
        check_read_u32(&f, ref_disk, n);

        f.nbytes = orig - (random() % blk_nbytes);
        for(unsigned i = 1; i < n; i++)
            check_read_n(&f, i, ref_disk, n, 0);

        output("passed all tests!\n");
    }
    output("DONE!  success!\n");
    return 0;
}
