for this high level version we use the actual rpi.h header

we define:

  - printk:

        int printk(const char *fmt, ...) {
            va_list ap;
            va_start(ap, fmt);
                int res = vprintf(fmt, ap);
            va_end(ap);
        
            return res;
        }

  - `clean_reboot`:

        void clean_reboot(void) {
            printk("DONE!!!\n");
            exit(0);
        }


The macros that the test programs use come from `libc/demand.h` (via
`rpi.h`).


Again:
  - don't need to define much.
  - can get a bunch of code for not much work.
