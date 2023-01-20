#ifndef __TRACE_H__
#define __TRACE_H__
// engler, cs140e: dirt simple tracing system.

// start tracing : 
//   <buffer_p> = 0: tells the code to immediately print the 
//                   trace.  
//   <buffer_p> != 0 defers the printing until <trace_stop> is called.
//
//  is an error: if you were already tracing.
void trace_start(int buffer_p);

// stop tracing
//  - error: if you were not already tracing.
void trace_stop(void);


// pause tracing: pause tracing, resume after calling
// trace_resume();
//
// only useful if you were running with buffering.
void trace_pause(void);
void trace_resume(void);


// make the above nicer so we can easily trace a function/chunk of code.
#define stringify(_x) #_x

#define trace_fn(fn) do {                                    \
    printk("about to trace: <%s>\n", stringify(fn));        \
    trace_start(0);                                         \
    fn;                                                     \
    trace_stop();                                           \
    printk("done: <%s>\n", stringify(fn));                  \
} while(0)

#endif
