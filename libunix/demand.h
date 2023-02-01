#ifndef __DEMAND_H__
#define __DEMAND_H__
// engler, cs140e.
// various helpful error-checking / debugging macros.
//
// You should work through and figure out what the different operators
// do.  E.g.,:
//  - gcc's variadic macros.
//  - using macros so you can print the file, function, linenumber 
//    that a usage occurs.
//  - the use of "do { ... } while()" to guard against possilbe problems
//    with macro-expansion.
//
// These techniques can 1/ clean up your code and 2/ make it easy
// to change the functionality of your code without modifications.
// 
// Of course, macro's are easily evil and gross (perhaps even these!),
// so caveat emptor.
//
// TODO: 
//  - refactor so there's not so much repetition.
//  - change debug to have a debug level?  
//  - change everything to use output() so you can just remap
//    one macro.
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

// use to "stringize" a macro argument.
#define _XSTRING(x) #x

// useful lab-macros


// same as panic, but differentiate that we literally thought this
// could not happen (not just that we were simply not handling a 
// case, etc.) maybe should refactor.
#define impossible(args...) do {                            \
    fprintf(stderr, "%s:%s:%d:IMPOSSIBLE ERROR:",           \
                        __FILE__, __FUNCTION__, __LINE__);  \
    fprintf(stderr, ##args);						        \
    exit(1);                                                \
} while(0)

// output to stderr without attaching the file/line.
#define output(msg...) fprintf(stderr, ##msg)

#define notreached()    panic("impossible: hit NOTREACHED!\n")
#define unimplemented() panic("this code is not implemented.\n")

/* Compile-time assertion: can only be used in a function. */
#define AssertNow(x) switch(1) { case (x): case 0: ; }




#define trace(msg...) do { output("TRACE:"); output(msg); } while(0)


// if you -DNDEBUG these go away
#ifdef NDEBUG
#   define demand(_expr, _msg, args...) do { } while(0)
#   define debug(msg...) do { } while(0)
#else

    // stolen from van wyk.
    // usage: 
    //	demand(expr, msg)
    // prints <msg> if <expr> is false.
    // example:
    //	demand(x < 3, x has bogus value!);
    // note: 
    //	if <_msg> contains a ',' you'll have to put it in quotes.
#   define demand(_expr, _msg, args...) do {			                \
      if(!(_expr)) { 							                        \
        fprintf(stderr, "%s:%s:%d: Assertion '<%s>' failed:",       \
                        __FILE__, __FUNCTION__, __LINE__, _XSTRING(_expr));           \
        fprintf(stderr, _XSTRING(_msg) "\n", ##args);	            \
        exit(1);						                            \
      }								                                \
    } while(0)

#   define debug_output(msg...) output("DEBUG:" msg)

// print a debug message; prepend file:function:linenumber.
#   define debug(msg...) do { 						                    \
        fprintf(stderr, "%s:%s:%d:", __FILE__, __FUNCTION__, __LINE__); \
        fprintf(stderr, ##msg);						                    \
    } while(0)
#endif

// various fatal error macros.

// called when <syscall> fails, print <msg> and the Unix perror value and die.
#define sys_die(syscall, msg, args...) do {                                         \
    debug("FATAL syscall error: " _XSTRING(msg) "\n\tperror reason: ", ##args);     \
    perror(_XSTRING(syscall));                                                      \
    exit(1);                                                                        \
} while(0)

#define sys_debug(syscall, msg) do {                                        \
    fprintf(stderr, "%s:%s:%d: <%s>\n\tperror=",                            \
                        __FILE__, __FUNCTION__, __LINE__, _XSTRING(msg));   \
    perror(_XSTRING(syscall));                                              \
} while(0)


// the called code cannot fail (<0)
#define no_fail(syscall) do {                                               \
    if((syscall) < 0) {                                                      \
        int reason = errno;                                                 \
        sys_die(syscall,                                                    \
            "<%s> failed: errno=%d: we thought this was impossible.\n",     \
                            _XSTRING(syscall),reason);                      \
    }                                                                       \
} while(0)

// print msg, exit cleanly.
#define clean_exit(msg...) do { 	\
    fprintf(stderr, ##msg);			\
    exit(0);                        \
} while(0)

// error: die without prepending information.
#define die(msg...) do { 						                            \
    fprintf(stderr, ##msg);						                            \
    exit(1);                                                                \
} while(0)

// die with a panic message; prepend file,function,linenumber.
#define panic(msg...) do { 						                            \
    output("%s:%s:%d:PANIC:", __FILE__, __FUNCTION__, __LINE__);   \
    die(msg); \
} while(0)

#define todo(msg) panic("TODO: %s\n", msg)

// same as panic, but no prefixed PANIC
#define fatal(msg...) do { 						                            \
    output("%s:%s:%d:", __FILE__, __FUNCTION__, __LINE__);   \
    die(msg); \
} while(0)

#endif /* __DEMAND_H__ */
