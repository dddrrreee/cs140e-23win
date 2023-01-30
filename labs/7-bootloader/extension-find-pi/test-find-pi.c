#include "libunix.h"

int main(int argc, char *argv[]) {
    if(argc != 2)
        die("%s: usage error: expects one argument have %d\n", argv[0], argc-1);

    const char *name = argv[1];
    const char *bin = find_pi_binary(name);
    if(!bin)
        output("did not find pi binary: <%s>\n", name);
    else {
        if(!exists(bin))
            panic("claimed to find <%s> but does not exist\n", name);
        output("found pi binary: <%s> at absoute path <%s>\n", name, bin);
    }
    return 0;
}
