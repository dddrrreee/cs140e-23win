#include "rpi.h"
#include "pi-sd.h"
#include "fat32.h"
#include "libc/fast-hash32.h"

static pi_file_t *
read_and_hash(fat32_fs_t *fs, pi_dirent_t *root, 
        char *name, uint32_t expected_hash) {

    printk("Looking for %s.\n", name);
    pi_dirent_t *d = fat32_stat(fs, root, name);
    if(!d)
        panic("%s: not found!\n", name);

    printk("Reading %s\n", name);
    pi_file_t *f = fat32_read(fs, root, name);
    assert(f);

    uint32_t hash = fast_hash(f->data,f->n_data);
    printk("crc of %s (nbytes=%d) = %x\n", name, f->n_data, hash);
    if(expected_hash && hash != expected_hash)
        panic("expected hash = %x, computed hash = %x\n", expected_hash, hash);

    return f;
}


void notmain() {
  uart_init();
  kmalloc_init();
  pi_sd_init();

  printk("Reading the MBR.\n");
  mbr_t *mbr = mbr_read();

  printk("Loading the first partition.\n");
  mbr_partition_ent_t partition;
  memcpy(&partition, mbr->part_tab1, sizeof(mbr_partition_ent_t));
  assert(mbr_part_is_fat32(partition.part_type));

  printk("Loading the FAT.\n");
  fat32_fs_t fs = fat32_mk(&partition);

  printk("Loading the root directory.\n");
  pi_dirent_t root = fat32_get_root(&fs);

#if 0
  printk("Listing files:\n");
  uint32_t n;
  pi_directory_t files = fat32_readdir(&fs, &root);
  printk("Got %d files.\n", files.ndirents);
  for (int i = 0; i < files.ndirents; i++) {
    if (files.dirents[i].is_dir_p) {
      printk("\tD: %s (cluster %d)\n", files.dirents[i].name, files.dirents[i].cluster_id);
    } else {
      printk("\tF: %s (cluster %d; %d bytes)\n", files.dirents[i].name, files.dirents[i].cluster_id, files.dirents[i].nbytes);
    }
  }
#endif

    // check that you can read data correctly.
    read_and_hash(&fs, &root, "BOOTCODE.BIN", 0xfd8d57d1);
    read_and_hash(&fs, &root, "START.ELF", 0xf36a80ce);

    // this could possibly fail if you have a different gcc
    // version --- change the cksum if so.
    read_and_hash(&fs, &root, "HELLO-F.BIN", 0x383a4198);

    // fill in the cksum values for these.
    printk("fill in the checksum for these three files\n");
    read_and_hash(&fs, &root, "BOOTLO~1.BIN", 0);
    read_and_hash(&fs, &root, "CONFIG.TXT", 0);
    read_and_hash(&fs, &root, "KERNEL.IMG", 0);
    panic("fill in the values for the previous three files\n");

    printk("PASS: %s\n", __FILE__);
    clean_reboot();
}
