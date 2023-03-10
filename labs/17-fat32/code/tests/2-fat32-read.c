#include "rpi.h"
#include "pi-sd.h"
#include "fat32.h"

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

  printk("Looking for config.txt.\n");
  char *name = "CONFIG.TXT";
  pi_dirent_t *config = fat32_stat(&fs, &root, name);
  demand(config, "config.txt not found!\n");

  printk("Reading config.txt.\n");
  pi_file_t *file = fat32_read(&fs, &root, name);

  printk("Printing config.txt (%d bytes):\n", file->n_data);
  printk("--------------------\n");
  for (int i = 0; i < file->n_data; i++) {
    printk("%c", file->data[i]);
  }
  printk("--------------------\n");

  printk("PASS: %s\n", __FILE__);
  clean_reboot();
}
