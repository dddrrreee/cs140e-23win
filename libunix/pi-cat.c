#include <ctype.h>
#include "libunix.h"

// hack-y state machine to indicate when we've seen the special string
// 'DONE!!!' from the pi telling us to shutdown.
int pi_done(unsigned char *s) {
    static unsigned pos = 0;
    const char exit_string[] = "DONE!!!\n";
    const int n = sizeof exit_string - 1;

    for(; *s; s++) {
        assert(pos < n);
        if(*s != exit_string[pos++]) {
            pos = 0;
            return pi_done(s+1); // check remainder
        }
        // maybe should check if "DONE!!!" is last thing printed?
        if(pos == sizeof exit_string - 1)
            return 1;
    }
    return 0;
}

// overwrite any unprintable characters with a space.
// otherwise terminals can go haywire/bizarro.
// note, the string can contain 0's, so we send the
// size.
void remove_nonprint(uint8_t *buf, int n) {
    for(int i = 0; i < n; i++) {
        uint8_t *p = &buf[i];
        if(isprint(*p) || (isspace(*p) && *p != '\r'))
            continue;
        *p = ' ';
    }
}

// read and echo the characters from the usbtty until it closes 
// (pi rebooted) or we see a string indicating a clean shutdown.
void pi_cat(int fd, const char *portname) {
    output("listening on ttyusb=<%s>\n", portname);

    while(1) {
        unsigned char buf[4096];
        int n = read(fd, buf, sizeof buf - 1);

        if(!n) {
            // this isn't the program's fault.  so we exit(0).
            if(tty_gone(portname))
                clean_exit("pi ttyusb connection closed.  cleaning up\n");
            // so we don't keep banginging on the CPU.
            usleep(1000);
        } else if(n < 0) {
            sys_die(read, "pi connection closed.  cleaning up\n");
        } else {
            buf[n] = 0;

            // if you keep getting "" "" "" it's b/c of the GET_CODE message from bootloader
            remove_nonprint(buf,n);
            output("%s", buf);

            if(pi_done(buf)) {
                output("\nSaw done\n");
                clean_exit("\nbootloader: pi exited.  cleaning up\n");
            }
        }
    }
    notreached();
}

