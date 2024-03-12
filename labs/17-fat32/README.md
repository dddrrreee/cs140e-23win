## Build a simple FAT32 read-only file system.

***NOTE***:
  - In a poetic irony, my laptop died last night and the m.2 ssd wouldn't
    boot a new spare laptop.  So the code in this lab is identical to 
    last year, based on a lab reworking by Akshay.   
  - Make sure you start with the [PRELAB](PRELAB.md)!
  - There is also a [FAT32 cheatsheet](./fat32-cheat-sheet.md).  You
    really want to read and use this.

  - I'd also recommend looking at the 
    [pjrc guide](https://www.pjrc.com/tech/8051/ide/fat32.html), although 
    it's a bit ambiguous/hard to understand in certain places.

***The first thing you do***:
- Run `make`.  Everything should compile (using a `staff-mbr.o` and
  `staff.fat32.o`), and it should automatically run a test of the MBR.  Let us
  know immediately if this doesn't work---some SD cards don't work with the SD
  driver we use.
    - Tests 0 and 1 should work.  Tests 2 will partially work, but some of them
      require you to copy files to the SD card first (described later in the README).
- Once the test works, change the Makefile to not use the staff object files.

We will do a fancy hello world: 
  1. Write a simple read-only file system that can read from your SD card.
  2. Use it to read in a `hello-f.bin` from the disk.
  3. Jump to it and run this program.

It's kind of cool that we can write a read-only FS for (arguably) the most
widely-used file system out there.  With that said, since we are using a 
pre-existing system, there is starter code:
  - `fat32-helpers.h`:  this has all of the data structures used by the FAT32 
    system and by the master boot record (you are welcome!). You should look 
    through this since there are all sorts of nifty things.  You already had 
    this from the PRELAB.
  - `fat32-helpers.c`: has various helpers, mostly for sanity checking and
    for printing.
  - `mbr-helpers.c`: has various helpers for the master boot record, mostly for 
    sanity checking and for printing.
  - `fat32-lfn-helpers.c`: this has the code to support "long file names" in 
    FAT32, which is a very gross hack that they did to keep things backwards 
    compatible.  I tried to isolate this code.  You are more than welcome to 
    write your own version (the wikipedia page is a good place to look).

There's no way around having this startup given how fancy the FAT32 structures
are, so it is what it is.  Fortunately you don't have to look at most of it
and the words for this lab are pretty short.  The starter code is pretty much 
all in the `*-helpers.[ch]` files, so you can use/ignore as much of it as you 
like (I'd suggest using at least the structures to get started, though).

The tl;dr strategy:
  1. use the annotated copy of Paul's writeup (in the `docs/` directory) and 
     the wikipedia page for further explanation.
  2. Look through `fat32.h`.
  3. Start coding in `fat32.c`.
  4. To get checksums, set `trace_p = 1` in `pi-sd.c` and it will emit `TRACE
     messages --- you can grep these out and compare them.

#### Checkoff

You should:
  1. Read and echo the contents of `config.txt` on your SD card.
  2. List all the files on your SD card.
  3. You should be able to swap SD cards with your partner and get the same (or
     similar) output.
  4. Be able to load/execute a "hello world" binary off the SD card.

#### Major extensions

There's easily a semester's worth of extensions.

  0. Rewrite the code (and interfaces!) to not allocate / leak so 
     much memory.  Also use arena allocation so when you unmount
     the file system everything gets freed.
  1. Be able to edit files on the SD card (e.g. pass the `3-` tests).
  2. Add (correct) caching to improve performance (using your `pread` from the prelab)
  3. Build a simple shell which lets you run `ls`, `cat`, etc. by typing in 
     commands.
  4. Add a second ram-based filesystem for temporary files (like /tmp).
  5. Add long file name support (we mostly just ignore them, but modern OSes 
     process them correctly).
  6. Rework the SD driver code so that it uses interrupts rather than
     being synchronous.  Similarly, add an asynchronous file system
     interface.

#### Debugging Tips

* Be very careful about sectors vs. clusters vs. bytes vs. blocks vs. directory
  entries.  When doing math, always do dimensional analysis to make sure the
  units work out.  Mixing up sectors and clusters was by far the single
  most common error.
* You can ignore LFNs until everything else works (or completely).  LFNs are a
  backwards-compatible hack, so it's technically allowed for a FAT32
  implementation to only ever handle short file names.  If you see a long file
  name entry, just skip it.

-------------------------------------------------------------------------
### Background: File systems over-simplified.

Hopefully this is redundant with the PRELAB.

Our r/pi's (and most SD card devices) expect to work with Microsoft's
FAT32 file system.  On the plus side, FAT32 is relatively simple and,
because it's so widely-used, fairly-well documented.  On the negative end,
it has a really ugly way to handling filenames longer than 8 characters.
(You can look at the LFN code in the `fat32-helper.c` to get a feel
for this).

The big picture:
1. From our view, disk is an array.  Each entry is a 512-byte sector.  The 
   first sector (again from our view, if not from the device's) is named 0, 
   the second, 1, etc.  To amortize overhead, devices let you read multiple 
   contiguous sectors with a single operation.

2. Over-simplifying: file systems are trees of files and directories.  
   Directories have pointers to files and directories.  Files have pointers 
   to data blocks.  For performance and space reasons, most file systems do 
   not represent files as contiguous arrays of pointers to data (as we 
   would with a simple one-level page table), but instead use an irregular 
   hierarchical structure of pointers to different types of structures that 
   eventually point to data blocks.  For example, the Unix FFS file system 
   uses an inode to hold the initial 12 or so pointers, an indirect block 
   to hold the next 1024 or so, double indirect blocks, etc.  You've seen 
   these tradeoffs and methods in page tables --- the different here is 
   that most files are small, and almost all files are accessed 
   sequentially from the beginning to the end, so the data structures to 
   navigate them are tuned differently.

3. If we had to only store these trees in in-memory, building a file system 
   would be a straightforward assignment in an intro programming class.  
   However, because we have to store the file system on disk, file systems were 
   the single largest systems research topic in the 80s and 90s.  (Mostly b/c 
   each access is/was millions of cycles, and you cannot write everything you 
   need atomically.)

   For our lab today, the main issue with disk storage is that pointers will be 
   more awkward since we have to use disk addresses instead of memory 
   addresses.  As a result, a pointer dereference becomes a bunch of function 
   calls and table calculations (similar, for real reasons, to what is going on 
   behind the scenes for virtual memory).  A related issue, is finding the 
   tables --- in our case, the FAT --- and understanding what its domain and 
   range is.

### Overview: FAT32.

Our SD cards are broken down as follows:

  - Sector 0 on the SD card: the master boot record (`mbr`, 512 bytes).
    Holds four partition tables giving the disk offset (in sectors)
    where the partition starts and the number of sectors in it.
    For us, the first partition should be as big as the SD card, and
    the rest should be empty.  The partition type should be FAT32.
    (Print and check.)

  - Sector 0 in the partion: the FAT32 boot sector (512 bytes).  FAT32 breaks the
    disk into many fields, some useful ones:
       - Number of sectors in each cluster (`sec_per_cluster`).
       - Total file system size (`nsec_in_fs`),
       - How many sectors the FAT table is stored in (`nsec_per_fat`).
       - `reserved_area_nsec` the number of blocks to skip before reading the 
       FAT table (which is the sector right after the reserved area).

    You can check the boot sector is valid (`fat32_volume_id_check`)
    and print it (`fat32_volume_id_print`).

   - The FAT table itself: read in `nsec_per_fat` sectors starting right after the
     `reserved_area_nsec`.  The FAT table is often replicated back-to-back, so you
     can read in both and check they are identical.

   - The root directory: located by reading `sec_per_cluster` worth of
     data from cluster 2.  

Further reading --- the first three are good summaries, the latter have more detail:

  1. [aeb's summary](https://www.win.tue.nl/~aeb/linux/fs/fat/fat-1.html).
  2. [forensic wiki](https://www.forensicswiki.org/wiki/FAT).
  3. [data structure layout](http://www.c-jump.com/CIS24/Slides/FileSysDataStructs/FileSysDataStructs.html).
  4.  [wikipedia](https://en.wikipedia.org/wiki/Design_of_the_FAT_file_system).
  A full description.  Perhaps more detail than you need, but useful
  for all the weird flags that can pop up.
  5. [linux source](https://elixir.bootlin.com/linux/latest/source/fs/fat/dir.c).

-------------------------------------------------------------------------
### Part 1: Read the MBR. (10 minutes)

Read the MBR:
  1. `tests/1-mbr.c` should be able to read in the master boot record.  It should pass
     our sanity check `mbr_check`.

-------------------------------------------------------------------------
### Part 2: read in the volume id (`fat32_boot_sec_t`) and `fsinfo`.  (10 minutes)

This should be fast:
  0. Implement the first parts of `fat32_mk` in `fat32.c`
  1. Use the partition entry (which the tests read from the MBR) to read the
     partition-specific boot sector (the volume id) off the SD card.  It's the
     first sector in the partition; there's a struct definition for it in
     `fat32_boot_sec_t`.
  2. Verify the boot sector with `fat32_volume_id_check`.
  3. Read in the `fsinfo` structure (which is usually right after the boot 
     sector, but you should confirm this is true on your SD card).
  4. Verify it with `fat32_fsinfo_check`.
  5. Print out the two sectors (we provided helpers for this) and make sure the 
     results are sensible.  Run the test with output; you won't pass the whole 
     thing until part 3, but check that the numbers you see make sense.


-------------------------------------------------------------------------
### Part 3: read in and setup the FAT (20 minutes)

Here you will use the boot record to setup the `fat32_fs_t` structure,
including the actual FAT table.  Use the volume ID and the partition to
implement the rest of the `fat32_mk` routine, which will define the important
pieces for your FAT32 FS.

A reasonable description of the information you need is
[here](https://www.pjrc.com/tech/8051/ide/fat32.html)

At the end of this, you should pass `tests/2-fat32-mk.c` completely.

-------------------------------------------------------------------------
### Part 4: read in the root directory (10 minutes)

You'll get the root directory, read it in, and print it.

   1. Use your fat structure to compute where cluster 2 is (the root 
      directory).
   2. Implement `fat32_get_root` to return it.
   3. Read it in (implement `fat32_readdir`).
   4. You should pass `tests/2-fat32-ls.c`, and the output should show you a 
      list of all the files in the root directory.

You can find the end of the root directory by looking for a "end of directory"
marker (a filename starting with a 0x00 byte).

Note that the root directory may be more than one sector long--you'll have to 
iterate through the FAT to find all the relevant sectors!  You'll probably want 
to implement the function `read_cluster_chain` to do this for you, since we'll 
use the same mechanic later.  You want to loop through the FAT until you find a 
cluster whose "next cluster" entry type is set to LAST_CLUSTER.

NOTE: do not  check the type of the fat entry by hand, in particular do not do 
a raw comparison to `LAST_CLUSTER`, since there are many allowed values for this
sentinel.   Instead, call `fat32_fat_entry_type` which strips out the bits
for you (you can certainly write a more clever version of this routine, 
which you are encouraged to do!):

        // in fat32-helpers.c
        int fat32_fat_entry_type(uint32_t x) {
            // eliminate upper 4 bits: error to not do this.
            x = (x << 4) >> 4;
            switch(x) {
            case FREE_CLUSTER:
            case RESERVED_CLUSTER:
            case BAD_CLUSTER:
                return x;
            }
            if(x >= 0x2 && x <= 0xFFFFFEF)
                return USED_CLUSTER;
            if(x >= 0xFFFFFF0 && x <= 0xFFFFFF6)
                panic("reserved value: %x\n", x);
            if(x >=  0xFFFFFF8  && x <= 0xFFFFFFF)
                return LAST_CLUSTER;
            panic("impossible type value: %x\n", x);
        }

-------------------------------------------------------------------------
### Part 5: read in and print `config.txt`

Only a few more steps to have a full read-only FAT32 driver:
   1. Implement `fat32_stat` to search for a file using its filename, and make 
      sure you can stat `CONFIG.TXT` in the root directory.
   2. Implement `fat32_read` to search for a file using its filename and read 
      in the contents.  Once you've done `stat` and `readdir`, this should be 
      pretty straightforward.  You'll want to use `read_cluster_chain` again to 
      read the whole file in at once.
   3. At this point, you should be able to pass all the tests in `tests/2-`.

-------------------------------------------------------------------------
### Part 6: read in and run `hello-f.bin`

   1. Store a `hello-f.bin` linked to a different address range onto your your SD card.
   2. Read it into a buffer at the required range.
   3. Jump to it.

Most of this code should be in `tests/2-fat32-jump.c` --- you'll have
to implement some of this to jump over the header.  The CRC's of the file
should be:

        crc of BOOTCODE.BIN (nbytes=50248) = 0xfd8d57d1
        crc of HELLO-F.BIN (nbytes=3384) = 0x7dd44481

-------------------------------------------------------------------------

### Part 7: Writing (Extension)

We've provided a template for turning your read-only driver into a read-write 
driver (this is what the `tests/3-` tests are for.  This is an extension for 
pragmatic reasons (i.e., it takes a while), but it's highly recommended since 
it'll let you do much more with your Pi.

Implement the following functions using the TODO comments in the code:

```C
int fat32_rename(fat32_fs_t *fs, pi_dirent_t *directory, char *oldname, char *newname);
pi_dirent_t *fat32_create(fat32_fs_t *fs, pi_dirent_t *directory, char *filename, int is_dir);
int fat32_delete(fat32_fs_t *fs, pi_dirent_t *directory, char *filename);
int fat32_truncate(fat32_fs_t *fs, pi_dirent_t *directory, char *filename, unsigned length);
int fat32_write(fat32_fs_t *fs, pi_dirent_t *directory, char *filename, pi_file_t *file);
```

Most of the work is actually in the helper function `write_cluster_chain`, which is similar to the `read_cluster_chain` you already implemented.

Once you're done, you should be able to pass all the `tests/3-` tests.  Add 
some of your own tests to make sure reading/writing works as expected.

Note that this is more complicated than reading, since now messing up means we 
might corrupt our SD card and make it unreadable on macOS or Linux (if this 
happens, reformat your SD card and follow the Lab 0 instructions to get 
everything back on it).

We have to be careful with the order of operations; if we (for example) delete 
a file from the disk before removing its directory entry, we could end up 
corrupting the disk if we crash.  Similarly, if we create a directory entry for 
a file and point it at a cluster we haven't marked as "in use" yet, we could 
get random garbage files if we crash.  Think carefully about how to do things 
to minimize the chances of corruption---remember that nothing here is atomic.
