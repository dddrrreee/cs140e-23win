
#ifndef __RPI_ASSERT_H__
#define __RPI_ASSERT_H__

#ifdef RPI_UNIX
#   error "should not get here"
#endif

#ifndef NDEBUG
#define debug(msg, args...) \
	(printk)("%s:%s:%d:" msg, __FILE__, __FUNCTION__, __LINE__, ##args)
#else
#define debug(msg, args...) do { } while(0)
#endif

#define output printk

#ifdef BACKTRACE
void backtrace_print(void);
#else
#define backtrace_print() output("[backtrace not defined in libpi/defs.mk]\n")
#endif

#define panic(msg, args...) do { 					\
	(printk)("PANIC:%s:%s:%d:" msg "\n", __FILE__, __FUNCTION__, __LINE__, ##args); \
    backtrace_print();\
	clean_reboot();							\
} while(0)


#define trace(args...) do { printk("TRACE:"); printk(args); } while(0)
#define trace_notreached() trace_panic("should not reach\n")

#define trace_panic(args...) do { \
    printk("TRACE:ERROR:"); \
    printk(args); \
	clean_reboot();							\
} while(0)
#define trace_clean_exit(args...) do { trace(args); clean_reboot(); } while(0)

#ifndef assert
#define assert(bool) do { if((bool) == 0) panic("%s", #bool); } while(0)
#endif

#define _XSTRING(x) #x

// usage: 
//      demand(expr, msg)
// note, if it doesn't take special characters does not need quotes:
//      demand(x, x is 0!);
// but you can always add them: 
//      demand(x, "x is not 0 = %d", x);
// prints <msg> if <expr> is false.
// example:
//      demand(x < 3, x has bogus value!);
// note: 
//      if <_msg> contains a ',' you'll have to put it in quotes.
#define demand(_expr, _msg, args...) do {                                        \
    if(!(_expr)) {                                                  \
        rpi_reset_putc();\
        (printk)("ERROR:%s:%s:%d: "                      \
                        "FALSE(<%s>): " _XSTRING(_msg) "\n",\
                        __FILE__, __FUNCTION__, __LINE__, _XSTRING(_expr), ##args);              \
        clean_reboot();\
    }                                                               \
} while(0)

/* Compile-time assertion used in function. */
#define AssertNow(x) switch(1) { case (x): case 0: ; }

#define unimplemented() panic("implement this function!\n");
#define not_reached() panic("NOTREACHED!\n");

#define exit_success(args...)  \
            do { printk("SUCCESS:"); output(args); clean_reboot(); } while(0)


// use this to panic if code runs more than once.
#define RUN_ONE_TIME() do {                 \
    static int init_p = 0;                  \
    if(init_p)                              \
        panic("called more than once!\n");  \
    init_p = 1;                             \
} while(0)

#endif
