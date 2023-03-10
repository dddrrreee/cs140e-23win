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
  pi_directory_t dir = fat32_readdir(&fs, &root);

  printk("Deleting TEMP.TXT\n");
  fat32_create(&fs, &root, "TEMP.TXT", 0);
  assert(fat32_delete(&fs, &root, "TEMP.TXT"));

  printk("PASS: %s\n", __FILE__);
  clean_reboot();
}
