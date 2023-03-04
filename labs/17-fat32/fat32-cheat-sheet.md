## (incomplete) fat32 cheatsheet 

I mostly ripped this off from "Understanding FAT32 Filesystems", Paul
Stoffregen.

The FAT wikipedia page is unusually clear and concrete.  I'd recommend
going through it.  You'll notice a few things we claim are always true
that are not necessarily so.  (Shouldn't matter today.  But goot to keep
in mind.)

--------------------------------------------------------------------------
### Definitions

Some (but not all) terms you need:

  - LBA: logical block address. LBA 0 is the start of the partition (where
    the master boot record is).

  - MBR: master boot record.  Describes the contents of upto four
    partitions, but almost always only one.

  - Type code: entry in the MBR that describes a partition type.  For
    FAT32 this is either `0xb` or `0xc`.

    Sort-of side note: In a programming language, you'd use static
    types to track types.  For on-disk, cross-language data we don't
    have static types and so use magic numbers (type codes) as a form
    of dynamic typing.  Same thing happens with network packets, linker
    files, etc. for similar reasons.

  - Sector: the smallest unit a disk can write.  Typically 512 bytes.
    Units smaller than a sector size (or straddling sectors) require
    a read-modify-write operation: read the sector, modify the bytes
    in it that you need, and write it out. This is analogous to
    read-modify-write when modifying a single bit in a memory byte.
    One conseqeunce of this feature is that it causes sub-sector writes
    to be slower than reads since they include both a read and a write.

    A sector generally has the guarantee that this write is *atomic*
    under power failure: it either happens completely or does not happen
    at all no matter when/if power failure (or system crash) occurs.

    Side note: since file systems have to be *persistent* (not go away
    when you turn off the power) they often have to wait until one or
    more (often many) sector writes complete.   As you would expect,
    a common cheat by storage companies and disk manufacturers is to lie
    about when these writes complete, since such lies makes them faster
    on benchmarks and typically go undetected.


  - Clusters: FAT (see below) files are made up of zero or more
    fixed number of contiguous sectors called *clusters*. (Thus,
    clusters are a file system constant multiple of the sector size.)
    Common sizes: 4k, 8k, 16k, 32k.  Most disk space in a FAT file system
    is consumed by clusters.

    Cluster size tradeoffs: The smaller the cluster, the more clusters
    you need both per file the larger the FAT will be for the same amount
    of file data.  Also, the smaller the maximal file size can be given
    a cluster ID pointer (e.g., 32-bits).

    Conversely, the larger the cluster larger your max file, the fewer
    pointers needed for a given file size and the smaller the FAT for
    a given partition size.  However, the more wasted disk/cache space
    (from *internal fragmentation*) and the more overhead from naive
    disk reads/writes,

    The issues are similar to page size.  The smaller the unit, the
    less internal fragmentation but the larger the table has to be.
    Issues are also similar to the size of the rectangles used to estimate
    area under curves in math.

  - FAT32: a FAT file system with 32-bit disk addresses.  
 
  - FAT: TLA of the unhelpful name "file allocation table".  

    FAT Intuition: a simple way to represent files would be as linked
    lists of sectors.  This is simple (good!) but requires O(n) disk
    accesses to get the nth block (read the first block to get the first
    next pointer, the second to get the second, etc).  It also generally
    makes reading/writing/caching sectors more expensive since you'd
    have to splice the next pointers in and out of the data.

    You can view the FAT as a clever  twist on this --- it pulls the
    linked list next pointers out into an array (the FAT) so that you
    don't have to mess with the file data at all and can typically cache
    the entire FAT in main memory.

    The FAT is indexed by Cluster ID.  Each FAT entry: the cluster ID
    of the next cluster.  So, for example if `fat[4] == 1234` that means
    the next cluster in the file containing cluster `4` is `1234`.

    Since each entry is 32-bits, a 512-byte sector holds 128 FAT32
    entries.

    To get all the data for a file or directory: iterate over its
    *cluster chain*.

  - LAST CLUSTER: Obviously for a linked list you need some kind of
    `NULL` terminator.  Just to confuse things, FAT32 uses any number
    greater than or equal to `0xFFFFFF8` rather than 0.

  - directory: a linear data structure of 32-byte records
    used to look up the starting cluster for each file or directory in
    the directory.  Only gives the starting cluster, not the rest of
    the links (you have to chase these through the FAT).

    Unused directory records are indicated by writing `0xe5` to the
    first byte.  The last directory entry has `0` as byte 0.
    (no other record has 0 as their first byte).

    Zero byte files have a `0` as their first cluster ID.

  - root directory: the root directory of the FAT32 file system
    tree (`/` in Unix).  It is given by "first cluster" field in the
    FAT32 boot sector.

  - FAT32 boot sector: cluster ID = 1.

  - `fat[0]` and `fat[1]`: hold special values.  Thus, the first legal
    cluster id is `2`.

  - LFN: long file name.  Normal files have a maximum 8 bytes
    for their name, an implicit '.'  then three bytes for "extension"
    (suffix).  Example: `boot.bin`.  LFN is a very ugly hack to extend
    file names in a backwards compatible way that won't break old code.


--------------------------------------------------------------------------
### tl;dr what to do

Roughly what we need to do:
  1. Read in the MBR by reading sector 0 (i.e., the single sector at disk
     address 0).  The `mbr_t` structure in `mbr.h` gives the layout.

  2. Get the disk address of the FAT32 boot sector (also known as the
     "volume id") so we need to determine where the file system starts
     along with other needed information (more below).

     For today: your SD card almost certainly has only one partition.
     So the first partiion in the MBR is what we want.  The LBA of the
     boot record is given by the `lba_start` field in the partition.

  3. Read in the FAT32 boot sector using the LBA given by `lba_start` in 
     step (2).

     The structure `fat32_boot_sec_t` in `fat32-helpers.h` defines the
     fields (among others):

       - `first_cluster` gives the cluster ID of the FAT32 root directory.
       - `nsecs_per_fat`: total sectors that hold the FAT. 
       - `nfats`: number of FAT tables (mirrored).
       - `sec_per_cluster`: the contiguous number of sectors in each cluster
         (file block).
       - `reserved_area_nsec`: number of reserved sectors before
         the FAT area starts.

#### How to convert a cluster ID to an LBA:

To convert a cluster ID to the LBA (the disk sector(s) you need to read)
we need two constants for the FAT32:

   - `sec_per_cluster`: the per-FAT constant given by the same named
     field in the FAT32 boot sector structure (above).

   - `cluster_begin_lba`: the LBA that the first cluster (cluster 2) starts at.
     All clusters are relative to this offset.  This starts at the first
     sector after the reserved data and after the FAT tables.  
       - sectors used for reserved data: `reserved_area_nsec`
       - total sectors used for a single FAT: `nsec_per_fat`.
       - total number of FATS: `nfat`.

     Just sum these up to get the first cluster begin lba.

Our forumula:

        lba(cluster_id) = cluster_begin_lba + (cluster_id - 2)* sec_per_cluster


How to follow cluster chains for a file `foo.bin`:
  1. Look up the name in the current directory.
  2. Get the first cluster id from this directory entry.
  3. While cluster id is not equal to `LAST_CLUSTER`:  

       1. convert the cluster ID to its LBA using the formula above.
       2. Read `sec_per_cluster` of data at this sector address.

The single most common mistake:
  - Not iterating through the entire file or directory.  Your test
    will work on a small file/directory (since they fit in one cluster)
    but not more than one.

--------------------------------------------------------------------------
#### Comments on speed 

Disk (even SSDs) are often the most expensive noun in our universe.
Bunch of different hacks to speed up.  An incomplete list:

  1. Often will have blocks cached in a buffer or block cache.  This is
     probably the single biggest speed improvement.  
  2. Given memory size, can cache entire FAT.  In this case
     can traverse over the entire file and issue a large single
     disk request (rather than multiple where you wait between them).
  3. Even if we don't have the entire FAT we can speculatively read more
     data than we need, on the hope that it was laid out contiguously.
     (Which you will want to do if at all possible so can issue very
     large sequential disk writes to write out the data.)

     This trick especially pays off on mechanical disks, which have an
     enormous fixed cost for each disk read, but a relatively smaller
     incremental cost per additional byte.

  4. Can periodically *compact* (*defragment*) the disk to make
     some-or-all file/directories contiguous.  Because each block/cluster
     is the same size, each cluster is interchangeable, and thus
     compaction is simpler to reason about as compared to heap garbage
     collection.  However, both share commonalities even though the
     communities don't have much to do with each other: for example,
     the usefulness of segregating blocks by lifetimes (*lifetime
     segregation*) to exploit the empirical fact that older data is less
     likely to die than newer.

There are *tons* of hacks to speed up file systems.  Many good final
projects!
