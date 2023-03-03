#include "rpi.h"

#define putchar rpi_putchar

static void emit_val(unsigned base, uint32_t u) {
    char num[33], *p = num;

    switch(base) {
    case 2:
        do {
            *p++ = "01"[u % 2];
        } while(u /= 2);
        break;
    case 10:
        do {
            *p++ = "0123456789"[u % 10];
        } while(u /= 10);
        break;
    case 16:
        do {
            *p++ = "0123456789abcdef"[u % 16];
        } while(u /= 16);
        break;
    default: 
        panic("invalid base=%d\n", base);
    }

    // buffered in reverse, so emit backwards
    while(p > &num[0]) {
        p--;
        putchar(*p);
    }
}

// a really simple printk. 
// need to do <sprintk>
int vprintk(const char *fmt, va_list ap) {
    for(; *fmt; fmt++) {
        if(*fmt != '%')
            putchar(*fmt);
        else {
            fmt++; // skip the %

            uint32_t u;
            int v;
            char *s;

            switch(*fmt) {
            case 'b': emit_val(2, va_arg(ap, uint32_t)); break;
            case 'u': emit_val(10, va_arg(ap, uint32_t)); break;
            case 'c': putchar(va_arg(ap, int)); break;

            // we only handle %llx.   
            case 'l':  
                fmt++;
                if(*fmt != 'l')
                    panic("only handling llx format, have: <%s>\n", fmt);
                fmt++;
                if(*fmt != 'x')
                    panic("only handling llx format, have: <%s>\n", fmt);
                putchar('0');
                putchar('x');
                // have to get as a 64 vs two 32's b/c of arg passing.
                uint64_t x = va_arg(ap, uint64_t);
                // we break it up otherwise gcc will emit a divide
                // since it doesn't seem to strength reduce uint64_t
                // on a 32-bit machine.
                uint32_t hi = x>>32;
                uint32_t lo = x;
                if(hi)
                    emit_val(16, hi);
                emit_val(16, lo);
                break;

            // leading 0x
            case 'x':  
            case 'p': 
                putchar('0');
                putchar('x');
                emit_val(16, va_arg(ap, uint32_t));
                break;
            // print '-' if < 0
            case 'd':
                v = va_arg(ap, int);
                if(v < 0) {
                    putchar('-');
                    v = -v;
                }
                emit_val(10, v);
                break;
            // string
            case 's':
                for(s = va_arg(ap, char *); *s; s++) 
                    putchar(*s);
                break;
            default: panic("bogus identifier: <%c>\n", *fmt);
            }
        }
    }
    return 0;
}

int printk(const char *fmt, ...) {
    va_list args;

    int ret;
    va_start(args, fmt);
       ret = vprintk(fmt, args);
    va_end(args);
    return ret;
}

#if 0
void notmain(void) {
    uint32_t v = 0xdeadbeef;
    new_printk("hello\n");
    new_printk("1234567890 = %d\n", 1234567890);
    new_printk("1234567890 = %u\n", 1234567890);
    new_printk("binary: %b\n", 0b101010111);
    new_printk("deadbeef=%x\n", v);
    new_printk("deadbeef=%p\n", v);
    new_printk("hello=<%s>\n", "hello");
}
#endif
