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

  printk("Looking for A.TXT\n");
  pi_dirent_t *a = fat32_stat(&fs, &root, "A.TXT");

  char *old = NULL;
  char *new = NULL;
  if (a) {
    printk("Found A.TXT!\n");
    old = "A.TXT";
    new = "B.TXT";
  } else {
    printk("Looking for B.TXT\n");
    pi_dirent_t *b = fat32_stat(&fs, &root, "B.TXT");
    if (b) {
      old = "B.TXT";
      new = "A.TXT";
    } else {
      panic("Found neither A.TXT nor B.TXT! Make sure you created one of them on the SD card!\n");
    }
  }
  printk("Renaming <%s> to <%s>\n", old, new);
  if (!fat32_rename(&fs, &root, old, new)) {
    panic("Unable to rename file!\n");
  }
  printk("PASS: %s\n", __FILE__);
  clean_reboot();
}
