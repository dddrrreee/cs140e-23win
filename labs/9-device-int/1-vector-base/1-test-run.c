// tests that your vector base actually works and how much speedup happens
// across the different methods using the <sys_plus1> system call in
// this directory's <interrupts-asm.S>
#include "rpi.h"
#include "rpi-interrupts.h"
#include "vector-base.h"
#include "cycle-count.h"

// in <interrupts-asm.S> in this directory
unsigned sys_plus1(unsigned);

// we try to prevent gcc inlining by putting the implementation
// after the callers.  since its in the same file at higher levels
// of optimization this might now be true (as usual: check the .list!)
void test_swi(const char *msg, void *vec);

void set_base(void *v);


static void vec_base_print(const char *msg, uint32_t *v) {
    output("%s: at addr=%x\n", msg, v);
    for(unsigned i = 0; i < 7; i++)
        output("\tvec[%d] = %x\n", i, v[i]);
}

void notmain(void) {
    // in interrupt-asm.S
    extern uint32_t _interrupt_vector_orig[];
    extern uint32_t _interrupt_vector_slow[];
    extern uint32_t _interrupt_vector_fast[];

    // NOTE: we don't have to setup interrupts since we are only 
    // doing SWI calls.
    vec_base_print("slow", _interrupt_vector_slow);
    vec_base_print("fast", _interrupt_vector_fast);
        
    test_swi("orig lab 5 vector(no cache)", _interrupt_vector_orig);
    test_swi("better relocation (no cache)", _interrupt_vector_slow);
    test_swi("fast relocation (no cache)", _interrupt_vector_fast);

    output("\n");
    trace("---------------------------------------------------------\n");
    trace("------------------- turning on caching ------------------\n");
    caches_enable();

    test_swi("orig lab 5 (no cache)", _interrupt_vector_orig);
    test_swi("better relocation (icache enabled)", _interrupt_vector_slow);
    test_swi("fast relocation (icache enabled)", _interrupt_vector_fast);
}

// this asssumes that gcc won't move the function call above or below the
// cycle count read.  you should check the .list to make sure!
void test_swi(const char *msg, void *v) {
    trace("%s\n", msg);
    set_base(v);

    // we use alignment to try to get rid of prefetch buffer effects.
    // should maybe include in the cycle count macro.
    asm volatile (".align 4");
    uint32_t s = cycle_cnt_read();
        sys_plus1(1); 
    uint32_t t = cycle_cnt_read() - s;
    trace("     single call took [%d] cycles\n", t);

    asm volatile (".align 4");
    s = cycle_cnt_read();
        sys_plus1(1); 
    t = cycle_cnt_read() - s;
    trace("     single call took [%d] cycles\n", t);

    asm volatile (".align 4");
    s = cycle_cnt_read();
        sys_plus1(1); 
    t = cycle_cnt_read() - s;
    trace("     single call took [%d] cycles\n", t);



    asm volatile (".align 4");
    s = cycle_cnt_read();
    sys_plus1(1); // 1
    sys_plus1(1); // 2
    sys_plus1(1); // 3
    sys_plus1(1); // 4
    sys_plus1(1); // 5
    sys_plus1(1); // 6
    sys_plus1(1); // 7
    sys_plus1(1); // 8
    sys_plus1(1); // 9
    sys_plus1(1); // 10
    t = cycle_cnt_read() - s;
    trace("     10 calls took [%d] cycles [%d] per call\n", t, t/10);
}

// install vector base, make sure what we expect is there,
// and do a few calls to make sure the system call works.
void set_base(void *v) {
    vector_base_reset(v);

    void *v_got = vector_base_get();
    if(v_got != v)
        panic("tried to set vector base <%p>: got <%p>\n", v,v_got);
   
    // should be randomized of course.
    enum { N = 20 };
    for(unsigned i = 0; i < N; i++) {
        uint32_t x = 0x12345678 + i;
        uint32_t x_got = sys_plus1(x);
        if((x + 1) != x_got)
            panic("ERROR: sys_plus1(%x) == %x\n", x, x_got);
    }
}
