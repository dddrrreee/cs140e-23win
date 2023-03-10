#ifndef __RPI_MBR_HELPERS_H__
#define __RPI_MBR_HELPERS_H__
/* 
 * engler, cs140e: useful structures for dealing with fat32 and sd cards.
 * the definitive source is the microsoft specification (which actually
 * isn't too bad and is fairly short).   but the links below give even
 * more concise summaries.
 *
 *      ****** DO NOT MODIFY THIS FILE ********
 *      ****** DO NOT MODIFY THIS FILE ********
 *      ****** DO NOT MODIFY THIS FILE ********
 *      ****** DO NOT MODIFY THIS FILE ********
 *      ****** DO NOT MODIFY THIS FILE ********
 */


/**********************************************************************
 * mbr + helpers.
 */

/*
  Copied from from: 
   https://www.win.tue.nl/~aeb/linux/fs/fat/fat-1.html

  layout:
    First the boot sector (at relative address 0), and possibly other
    stuff. Together these are the Reserved Sectors. Usually the boot
    sector is the only reserved sector.

    Then the FATs (following the reserved sectors; the number of reserved
    sectors is given in the boot sector, bytes 14-15; the length of a
    sector is found in the boot sector, bytes 11-12).

    Then the Root Directory (following the FATs; the number of FATs is
    given in the boot sector, byte 16; each FAT has a number of sectors
    given in the boot sector, bytes 22-23).

    Finally the Data Area (following the root directory; the number of
    root directory entries is given in the boot sector, bytes 17-18,
    and each directory entry takes 32 bytes; space is rounded up to
    entire sectors).
*/



/*
Byte Range | Description              | Essential
0-445      | Boot Code                | No
446-461    | Partition Table Entry #1 | Yes
462-477    | Partition Table Entry #2 | Yes
478-493    | Partition Table Entry #3 | Yes
494-509    | Partition Table Entry #4 | Yes
510-511    | Signature value (0xAA55) | Yes

Note: you can ignore "code"; on an x86 PC, this is where the bootloader would
go, but as you know the Pi uses "kernel.img" instead of this.  This is actually
a really nice feature of the Pi---otherwise, we'd need to fit the entire
bootloader into 446 bytes!

Search the partition table for a partition with the type code 0x0B or 0x0C.
This is far more convenient if you copy part_tab1 into a `struct
partition_entry` below.  You have to copy (not cast) because the entries are
annoyingly not aligned.
*/
typedef struct mbr {
    uint8_t     code[446],
                part_tab1[16],
                part_tab2[16],
                part_tab3[16],
                part_tab4[16];
    uint16_t sigval;
} mbr_t;
_Static_assert(sizeof(struct mbr) == 512, "mbr size wrong");

// partition entry.
typedef struct partition_entry {
    uint32_t bootable_p:8,
             chs_start:24,
             part_type:8,
             chs_end:24;
    uint32_t lba_start;
    uint32_t nsec;
} __attribute__ ((packed)) mbr_partition_ent_t;
_Static_assert (sizeof(struct partition_entry) == 16, "partition_entry: size wrong");

// check if a `part_type` corresponds to FAT32
static inline int mbr_part_is_fat32(int t) { return t == 0xb || t == 0xc; }

// return pointer to partition in <mbr> (0..3), error if partno>=4.
mbr_partition_ent_t mbr_get_partition(struct mbr *mbr, unsigned partno);

// check that the master boot record looks as we expect.
void mbr_check(struct mbr *mbr);

// print out the partition <p>
void mbr_partition_print(const char *msg, struct partition_entry *p);

// is partition empty?
int mbr_partition_empty(mbr_partition_ent_t *part);
int mbr_partition_empty_raw(uint8_t *part);

#endif
