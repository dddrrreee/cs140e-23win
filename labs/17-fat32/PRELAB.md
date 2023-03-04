### Overview
We're going to be working with the FAT32 filesystem for this lab. As you 
learned in CS110, there are a lot of different filesystems, each with its own 
pros and cons.  However, the Pi insists that our SD card uses FAT32, which kind 
of forces us to do the same for our kernel. Note that _technically_, we could 
just have a tiny FAT32 partition at the beginning of the SD card and do our own 
thing afterwards, like Linux does, but that adds yet another moving part to 
deal with.  For simplicity, we're just going to use one partition for the whole 
disk.

--------------------------------------------------------------------------
### Reading
  - [paul's summary](https://www.pjrc.com/tech/8051/ide/fat32.html).  This has
    some useful pictures.   An [annotated version](./docs/pauls-fat32.annoted.pdf)
    is in the `docs/` directory: this is a good place to start.
  -  [wikipedia](https://en.wikipedia.org/wiki/Design_of_the_FAT_file_system).
     full description.  Perhaps more detail than you need, but useful
     for all the weird flags that can pop up.   
     An [annotated version](./docs/wikipedia-fat32.annotated.pdf) is in the `docs/` directory.  It is a good companion to Paul's
    writeup.  It has the most thorough LFN discussion.
  - [aeb's summary](https://www.win.tue.nl/~aeb/linux/fs/fat/fat-1.html).
  - [data structure layout](http://www.c-jump.com/CIS24/Slides/FileSysDataStructs/FileSysDataStructs.html) this has useful tables.
  - [cpl.li's writeup](https://cpl.li/2019/mbrfat/) -- more pictures.  Some
    extra sentences.
  - [OSDev's partition writeup](https://wiki.osdev.org/Partition_Table).

  - [linux source](https://elixir.bootlin.com/linux/latest/source/fs/fat/dir.c).
  - The file system implementation chapter from the "three easy pieces" book 
    (in the `docs/` directory).
  - Microsofts specification (in the `docs/` directory).
  - [forensic wiki](https://www.forensicswiki.org/wiki/FAT) --- seems to be gone.

If you're confused by "little-endian" versus not:
  - [little endian](https://en.wikipedia.org/wiki/Endianness#Little-endian).

Once you've done these readings, you should:
- know the differences between a sector, a cluster, a volume, and a partition
- be able to verify that a disk has a FAT32 filesystem on it
- be able to find the root directory of a FAT32 filesystem
- be able to find the first cluster of a file within a directory
- be able to find the full contents of a file given the index of the first 
  cluster
