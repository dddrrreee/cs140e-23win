#include "rpi.h"
#include "pi-sd.h"
#include "fat32.h"
#include "libc/fast-hash32.h"

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

    char *name = "BOOTCODE.BIN";
    printk("Looking for %s.\n", name);
    pi_dirent_t *d = fat32_stat(&fs, &root, name);
    demand(d, "bootcode.bin not found!\n");

    printk("Reading %s\n", name);
    pi_file_t *f = fat32_read(&fs, &root, name);
    assert(f);

    printk("crc of %s (nbytes=%d) = %x\n", name, f->n_data, 
            fast_hash(f->data,f->n_data));

    name = "HELLO-F.BIN";
    printk("Looking for %s.\n", name);
    d = fat32_stat(&fs, &root, name);
    demand(d, "hello.bin not found!\n");

    printk("Reading %s\n", name);
    f = fat32_read(&fs, &root, name);
    assert(f);

    printk("crc of %s (nbytes=%d) = %x\n", name, f->n_data, 
            fast_hash(f->data,f->n_data));

    uint32_t *p = (void*)f->data;
    for(int i = 0; i < 4; i++) 
        printk("p[%d]=%x (%d)\n", i,p[i],p[i]);

    // magic cookie at offset 0.
    assert(p[0] == 0x12345678);

    // address to copy at is at offset 2
    uint32_t addr = p[2];
    assert(addr == 0x100000f0);

    // jump to it using BRANCHTO.  make sure
    // you skip the header!  (see in hello-f.list
    // and memmap.fixed in 13-fat32/hello-fixed
    unimplemented();

  printk("PASS: %s\n", __FILE__);
  clean_reboot();
}
