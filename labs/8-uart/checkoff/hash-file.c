#include "libunix.h"

int main(int argc, char *argv[]) {
    // const char *name = "../../7-bootloader/2-pi-side/kernel.img";
    const char *name = argv[1];

    assert(argv[1]);
    unsigned nbytes;
    uint8_t *code = read_file(&nbytes, name);
    uint32_t hash = fast_hash(code, nbytes);
    output("HASH: crc of file=<%s>:\n\tNBYTES=%d\n\tHASH=%x\n",
            name, nbytes, hash);
    return 0;
}
