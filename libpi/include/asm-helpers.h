#ifndef __ASM_HELPERS_H__
#define __ASM_HELPERS_H__

/*
 * abuse C's macro system to generate inline assembly routines 
 * given the format close to that used in the manual.
 *
 * <cp_asm_set> creates a routine that writes the given co-proc
 * register.  it appends a <_set> suffix to the provided name.
 * it also issues a <prefetch_flush> to flush the instruction
 * pipeline (needed for most by default).
 *
 * <cp_asm_set_raw> works the same, but no <prefetch_flush>
 *
 * ,cp_asm_get> creates a routine to read the given 
 * co-processor register.  it appends a <_get> suffix to 
 * the provided name.
 *
 * Examples:
 *   to create <void tcm_data_region_set(uint32_t v)>:
 *      cp_asm_set(tcm_data_region, p15, 0, c9, c1, 0)
 *   to create <uint32_t tcm_data_region_get(void)>:
 *      cp_asm_get(tcm_data_region, p15, 0, c9, c1, 0)
 *
 * NOTE: we should probably have a raw version that does not 
 * prepend the names.
 */


static inline void prefetch_flush(void) {
    unsigned r = 0;
    asm volatile ("mcr p15, 0, %0, c7, c5, 4" :: "r" (r));
}

// ugly: have two versions: the *_fn macros expand without
// "static inline" the others with "static inline"  
// cleaner to define a return type, but that interferes w/ some
// things.

// turn <x> into a string
#define MK_STR(x) #x
#define cp_asm_set_raw_fn(fn_name, coproc, opcode_1, Crn, Crm, opcode_2)   \
    void fn_name ## _set_raw(uint32_t v) {                    \
        asm volatile ("mcr " MK_STR(coproc) ", "                        \
                             MK_STR(opcode_1) ", "                      \
                             "%0, "                                     \
                            MK_STR(Crn) ", "                            \
                            MK_STR(Crm) ", "                            \
                            MK_STR(opcode_2) :: "r" (v));               \
    }

// no prefetch flush.
#define cp_asm_set_raw(fn_name, coproc, opcode_1, Crn, Crm, opcode_2)   \
    static inline cp_asm_set_raw_fn(fn_name, coproc, opcode_1, Crn, Crm, opcode_2)


// define a general co-processor inline assembly routine to set the value.
// from manual: must prefetch-flush after each set.
#define cp_asm_set(fn_name, coproc, opcode_1, Crn, Crm, opcode_2)       \
    cp_asm_set_raw(fn_name, coproc, opcode_1, Crn, Crm, opcode_2)       \
    static inline void fn_name ## _set(uint32_t v) {                    \
        fn_name ## _set_raw(v);                                         \
        prefetch_flush();                                               \
    }

#define cp_asm_set_fn(fn_name, coproc, opcode_1, Crn, Crm, opcode_2)       \
    cp_asm_set_raw_fn(fn_name, coproc, opcode_1, Crn, Crm, opcode_2)       \
    void fn_name ## _set(uint32_t v) {                    \
        fn_name ## _set_raw(v);                                         \
        prefetch_flush();                                               \
    }

#define cp_asm_get_fn(fn_name, coproc, opcode_1, Crn, Crm, opcode_2)       \
    uint32_t fn_name ## _get(void) {                      \
        uint32_t ret=0;                                                   \
        asm volatile ("mrc " MK_STR(coproc) ", "                        \
                             MK_STR(opcode_1) ", "                      \
                             "%0, "                                     \
                            MK_STR(Crn) ", "                            \
                            MK_STR(Crm) ", "                            \
                            MK_STR(opcode_2) : "=r" (ret));             \
        return ret;                                                     \
    }

#define cp_asm_get(fn_name, coproc, opcode_1, Crn, Crm, opcode_2)       \
        static inline cp_asm_get_fn(fn_name, coproc, opcode_1, Crn, Crm, opcode_2)

#define cp_asm(fn_name, coproc, opcode_1, Crn, Crm, opcode_2)   \
    cp_asm_set(fn_name, coproc, opcode_1, Crn, Crm, opcode_2) \
    cp_asm_get(fn_name, coproc, opcode_1, Crn, Crm, opcode_2) 

#define cp_asm_fn(fn_name, coproc, opcode_1, Crn, Crm, opcode_2)   \
    cp_asm_set_fn(fn_name, coproc, opcode_1, Crn, Crm, opcode_2) \
    cp_asm_get_fn(fn_name, coproc, opcode_1, Crn, Crm, opcode_2) 

#define cp_asm_raw(fn_name, coproc, opcode_1, Crn, Crm, opcode_2)   \
    cp_asm_set_raw(fn_name, coproc, opcode_1, Crn, Crm, opcode_2) \
    cp_asm_get(fn_name, coproc, opcode_1, Crn, Crm, opcode_2) 

#endif
