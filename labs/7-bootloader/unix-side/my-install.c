// engler, cs140e: driver for "bootloader" for an r/pi connected via 
// a tty-USB device.
//
// To make grading easier:
//              DO NOT MODIFY THIS CODE!
//              DO NOT MODIFY THIS CODE!
//              DO NOT MODIFY THIS CODE!
//              DO NOT MODIFY THIS CODE!
//              DO NOT MODIFY THIS CODE!
//
// You shouldn't have to modify any code in this file.  Though, if you find
// a bug or improve it, let me know!
//
#include <ctype.h>
#include <stdarg.h>
#include <string.h>
#include <sys/stat.h>
#include <termios.h>

#include "libunix.h"
#include "simple-boot.h"

int trace_p = 0; 
static char *progname = 0;

static void usage(const char *msg, ...) {
    va_list args;
    va_start(args, msg);
    vfprintf(stderr, msg, args);
    va_end(args);

    output("\nusage: %s  [--trace] ([device] | [--last] | [--first] [--device <device>]) <pi-program> \n", progname);
    output("    pi-program = has a '.bin' suffix\n");
    output("    specify a device using any method:\n");
    output("        <device>: has a '/dev' prefix\n");
    output("       --last: gets the last serial device mounted\n");
    output("        --first: gets the first serial device mounted\n");
    output("        --device <device>: manually specify <device>\n");
    exit(1);
}

int main(int argc, char *argv[]) { 
    char *dev_name = 0;
    char *pi_prog = 0;

    progname = argv[0];
    for(int i = 1; i < argc; i++) {
        if(strcmp(argv[i], "--trace") == 0)  {
            trace_p = 1;
        } else if(strcmp(argv[i], "--last") == 0)  {
            if(dev_name)
                usage("used --last but already have device: <%s>\n", dev_name);
            dev_name = find_ttyusb_last();
        } else if(strcmp(argv[i], "--first") == 0)  {
            if(dev_name)
                usage("used --first but already have device: <%s>\n", dev_name);
            dev_name = find_ttyusb_first();
        // we assume: anything that begins with /dev/ is a device.
        } else if(prefix_cmp(argv[i], "/dev/")) {
            if(dev_name)
                usage("specfied device <%s> but already have device: <%s>\n",
                    argv[i], dev_name);
            dev_name = argv[i];
        // we assume: anything that ends in .bin is a pi-program.
        } else if(suffix_cmp(argv[i], ".bin")) {
            if(pi_prog)
                usage("specified pi prog <%s> but already have=<%s>\n", 
                    argv[i], pi_prog);
            pi_prog = argv[i];
        } else if(strcmp(argv[i], "--device") == 0) {
            i++;
            if(dev_name)
                usage("used --device %s but already have device: <%s>\n", 
                    argv[i], dev_name);
            dev_name = argv[i];
            assert(dev_name);
        } else {
            usage("unexpected argument=<%s>\n", argv[i]);
        }
    }
    if(!pi_prog)
        usage("no pi program\n");

    output("dev name=<%s>\n", dev_name);
    output("pi=<%s>\n", pi_prog);
    output("trace=<%d>\n", trace_p);
    exit(0);

    // 1. get the name of the ttyUSB.
    if(!dev_name) {
        dev_name = find_ttyusb_last();
        if(!dev_name)
            panic("didn't find a device\n");
    }

    // 2. open the ttyUSB in 115200, 8n1 mode
    int fd = set_tty_to_8n1(open_tty(dev_name), B115200, 1);

    // 3. read in program
	unsigned nbytes;
    uint8_t *code = read_file(&nbytes, pi_prog);

    // 4. let's send it!
	output("%s: tty-usb=<%s> program=<%s> about to boot\n", progname, dev_name, pi_prog);
    simple_boot(fd, code, nbytes);

    // 5. echo output from pi
    pi_echo(0, fd, dev_name);
	return 0;
}
