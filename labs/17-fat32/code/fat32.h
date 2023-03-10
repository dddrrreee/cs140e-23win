#ifndef __RPI_FAT32_H__
#define __RPI_FAT32_H__
#include "mbr.h"
#include "pi-files.h"

/*
 * Aggregate the FAT32 information.  Refer to Paul's writeup for
 * how to compute thes.   you can compute each using either:
 *    1. the "volume id" (our <fat32_boot_sec_t> struct in fat32-helpers.h)
 *    2. or the boot partition (should be partition 0 of the <mbr>
 *
 * to access a cluster:
 *      cluster lba = cluster_begin + (cluster_number - 2) * sectors_per_cluster
 */
typedef struct fat32 {
  uint32_t
    lba_start,                  // start of partition
    fat_begin_lba,              // start of partition + nreserved sectors
    cluster_begin_lba,          // above + (number of FATs * nsec per FAT)
    sectors_per_cluster,
    root_dir_first_cluster,     // lba of first_cluster
    *fat,                       // pointer to in-memory copy of FAT
    n_entries;                  // number of entries in the FAT table.
} fat32_fs_t;

// Create a new FAT32 FS object, validating that the specified partition is a
// FAT32 partition.
fat32_fs_t fat32_mk(mbr_partition_ent_t *partition);

// Create a dirent for the root directory, suitable for passing to
// fat32_readdir.
pi_dirent_t fat32_get_root(fat32_fs_t *fs);

// Return a directory listing for the specified directory.
//  - store the number of directory entries in `dir_n`
pi_directory_t fat32_readdir(fat32_fs_t *fs, pi_dirent_t *dirent);

// Look up a specific file (by name) in a directory, and return the
// corresponding dirent.
pi_dirent_t *fat32_stat(fat32_fs_t *fs, pi_dirent_t *directory, char *filename);

// Read a file into memory and return it.
pi_file_t *fat32_read(fat32_fs_t *fs, pi_dirent_t *directory, char *filename);

// Rename a file's directory entry (on disk).  Pass in the dirent of the parent
// directory, *not* of the file itself.
int fat32_rename(fat32_fs_t *fs, pi_dirent_t *directory, char *oldname, char *newname);

// Create a new directory entry for an empty file.
pi_dirent_t *fat32_create(fat32_fs_t *fs, pi_dirent_t *directory, char *filename, int is_dir);

// Delete a file, including its directory entry.
int fat32_delete(fat32_fs_t *fs, pi_dirent_t *directory, char *filename);

// Truncate a file (change its length without changing its data, either padding
// with zeros or cutting off the end)
int fat32_truncate(fat32_fs_t *fs, pi_dirent_t *directory, char *filename, unsigned length);

// Write a file to the disk, using the specified directory.  Truncate the
// file if necessary.
int fat32_write(fat32_fs_t *fs, pi_dirent_t *directory, char *filename, pi_file_t *file);

// Flush any queued changes to the disk.  Will be a no-op if you don't do any
// caching (i.e., on your first version), but may be useful later if you try to
// speed things up.
int fat32_flush(fat32_fs_t *fs);

#endif
