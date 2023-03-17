#include <stdint.h>
struct prog {
    const char *name;
    unsigned nbytes;
    uint8_t data[];
};
