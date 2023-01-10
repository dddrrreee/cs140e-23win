int barrier_store(int *p) { 
    *p = 1;
    asm volatile ("" : : : "memory");
    return *p;
}

int no_barrier_store(int *p) { 
    *p = 1;
    return *p;
}

extern void bar(void);


// p escapes: forces to memory and back.
int call_ptr(int *p) {
    *p = 1;
    bar();
    return *p;
}

// x does not escape: replaces with 1.
int call_local(void) {
    int x = 1;
    bar();
    return x;
}


// gcc does not obey no inline??
__attribute__ ((__noinline__)) unsigned disable(void) {
    // disable interrupts
    unsigned t;
    asm volatile("mrs %0,cpsr; orr %0,%0,#(1<<7); msr cpsr_c,%0" : "=r"(t));
    return t;
}
__attribute__ ((__noinline__)) unsigned enable(void) {
    // enable interrupts
    unsigned t;
    asm volatile("mrs %0,cpsr; bic %0,%0,#(1<<7); msr cpsr_c,%0" : "=r"(t));
    return t;
}

int call_static(void) {
    static int y = 1;

    disable();
    y += 1;
    enable();

    disable();
    y += 1;
    enable();

    disable();
    y += 1;
    enable();

    return y;
}

volatile unsigned sw_int_is_on = 1;

__attribute__ ((__noinline__)) unsigned sw_int_off(void) {
    unsigned t = sw_int_is_on;
    sw_int_is_on = 0;
    return t;
}
__attribute__ ((__noinline__)) void sw_int_on(void) {
    // enable interrupts
    sw_int_is_on = 1;
}

int call_static_sw_int(void) {
    static int y = 1;

    sw_int_off();
    y += 1;
    sw_int_on();

    sw_int_off();
    y += 1;
    sw_int_on();

    sw_int_off();
    y += 1;
    sw_int_on();

    return y;
}

__attribute__ ((__noinline__)) void b(void) {
    asm volatile ("" : : : "memory");
}


int static_barrier(void) {
    static int y = 1;

    b();
    y += 1;
    b();

    b();
    y += 1;
    b();

    b();
    y += 1;
    b();

    return y;
}

unsigned y;
int global_barrier(void) {

    b();
    y += 1;
    b();

    b();
    y += 1;
    b();

    b();
    y += 1;
    b();

    return y;
}

// one last one: push stuff on a "stack" and then restore
// it.   cogo
//
// semantics of the other is a thread fork with a shared
// stack
