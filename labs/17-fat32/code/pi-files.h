#ifndef __RPI_FILES_H__
#define __RPI_FILES_H__

// A memory representation of a file.
// There are better ways to do this (which don't involve reading the entire
// file into RAM), but this is a simple way to start.
typedef struct {
  char *data;
  size_t n_alloc,    // total bytes allocated.
         n_data;     // how many bytes of data
} pi_file_t;

// A memory representation of a directory entry.
typedef struct {
  char name[16], raw_name[16];
  uint32_t cluster_id, is_dir_p, nbytes;
} pi_dirent_t;

// A directory listing is an array of directory entries.
typedef struct {
  pi_dirent_t *dirents;
  unsigned ndirents;
} pi_directory_t;

#endif
