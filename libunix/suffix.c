#include <string.h>
#include "libunix.h"
#include "staff-libunix.h"

int suffix_cmp(const char *s, const char *suffix) {
    int n0 = strlen(s), n1 = strlen(suffix);
    if(n0 < n1)
        return 0;
    s += n0 - n1;
    return strcmp(s, suffix) == 0;
}


int prefix_cmp(const char *s, const char *prefix) {
    int n0 = strlen(s), n1 = strlen(prefix);
    if(n0 < n1)
        return 0;
    return strncmp(s, prefix, n1) == 0;
}
