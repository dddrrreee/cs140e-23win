#ifndef __USER_PROGS_H__
#define __USER_PROGS_H__

void hello(void);
void vector(void);
void nop_10(void);
void mov_add(void);
void nop_1(void);
void mov_ident(void);

#if 0
#ifdef PROG_DEFS
    struct prog {
        void (*prog)(void);
        unsigned stack_p;
        uint32_t expected_hash;
    } progs[] = {
        { .prog = hello,        .stack_p = 1 },
        // { .prog = vector,        .stack_p = 1 },
        { .prog = nop_10,       .stack_p = 0,   .expected_hash = 0xaa81edbf },
        { .prog = nop_1,        .stack_p = 0,    .expected_hash = 0xbfde46be },
        { .prog = mov_ident,    .stack_p = 0,   .expected_hash = 0xcd6e5626 },
        { .prog = mov_add,      .stack_p = 0,   .expected_hash = 0xcacb2f6 },
        {}
    };
#endif
#endif

#endif
