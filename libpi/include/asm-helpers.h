#ifndef __ASM_HELPERS_H__
#define __ASM_HELPERS_H__

// static inline void prefetch_flush_set(uint32_t r);
static inline void prefetch_flush(void) {
    unsigned r = 0;
    asm volatile ("mcr p15, 0, %0, c7, c5, 4" :: "r" (r));
}


// turn <x> into a string
#define MK_STR(x) #x

// define a general co-processor inline assembly routine to set the value.
// from manual: must prefetch-flush after each set.
#define cp_asm_set(fn_name, coproc, opcode_1, Crn, Crm, opcode_2)       \
    static inline void fn_name ## _set(uint32_t v) {                    \
        asm volatile ("mcr " MK_STR(coproc) ", "                        \
                             MK_STR(opcode_1) ", "                      \
                             "%0, "                                     \
                            MK_STR(Crn) ", "                            \
                            MK_STR(Crm) ", "                            \
                            MK_STR(opcode_2) :: "r" (v));               \
        prefetch_flush();                                               \
    }

// no prefetch flush.
#define cp_asm_set_raw(fn_name, coproc, opcode_1, Crn, Crm, opcode_2)   \
    static inline void fn_name ## _set_raw(uint32_t v) {                    \
        asm volatile ("mcr " MK_STR(coproc) ", "                        \
                             MK_STR(opcode_1) ", "                      \
                             "%0, "                                     \
                            MK_STR(Crn) ", "                            \
                            MK_STR(Crm) ", "                            \
                            MK_STR(opcode_2) :: "r" (v));               \
    }

#define cp_asm_get(fn_name, coproc, opcode_1, Crn, Crm, opcode_2)       \
    static inline uint32_t fn_name ## _get(void) {                      \
        uint32_t ret=0;                                                   \
        asm volatile ("mrc " MK_STR(coproc) ", "                        \
                             MK_STR(opcode_1) ", "                      \
                             "%0, "                                     \
                            MK_STR(Crn) ", "                            \
                            MK_STR(Crm) ", "                            \
                            MK_STR(opcode_2) : "=r" (ret));             \
        return ret;                                                     \
    }


#define cp_asm(fn_name, coproc, opcode_1, Crn, Crm, opcode_2)   \
    cp_asm_set(fn_name, coproc, opcode_1, Crn, Crm, opcode_2) \
    cp_asm_get(fn_name, coproc, opcode_1, Crn, Crm, opcode_2) 

#define cp_asm_raw(fn_name, coproc, opcode_1, Crn, Crm, opcode_2)   \
    cp_asm_set_raw(fn_name, coproc, opcode_1, Crn, Crm, opcode_2) \
    cp_asm_get(fn_name, coproc, opcode_1, Crn, Crm, opcode_2) 


// Example to define a tcm_data_region_get/set:
// cp_asm_set(tcm_data_region, p15, 0, c9, c1, 0)
// cp_asm_get(tcm_data_region, p15, 0, c9, c1, 0)
#endif
