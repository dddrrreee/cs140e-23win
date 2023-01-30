#include <string.h>
#include "libunix.h"

int main(void) {
    const char *name = "./read-file.out";
    const char *hello = "hello";
    
    int fd = create_file(name);
    int i;
    for(i = 0; hello[i]; i++)
        write_exact(fd, &hello[i], 1);
    // write 0
    write_exact(fd, &hello[i], 1);
    close(fd);

    unsigned nbytes;
    char *data = read_file(&nbytes, name);

    unsigned n = strlen(hello) + 1;
    if(nbytes != n)
        panic("expected %d bytes, have %d\n", n, nbytes);
    else
        output("success: expected %d bytes, have %d\n", n, nbytes);

    if(strcmp(hello, data) != 0)
        panic("expected string <%s> have <%s>\n", hello,data);
    else
        output("expected string <%s> have <%s>\n", hello,data);

    run_system("rm -f %s", name);
    output("read-file: success [should test other sizes!]\n");
    return 0;
}
