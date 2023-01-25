#ifndef __MEMMAP_H__
#define __MEMMAP_H__

// symbols defined in libpi/memmap
extern uint32_t  __code_start__[];
extern uint32_t  __code_end__[];

extern uint32_t  __data_start__[];
extern uint32_t  __data_end__[];

extern uint32_t  __bss_start__[];
extern uint32_t  __bss_end__[];

extern uint32_t  __prog_end__[];
extern uint32_t  __heap_start__[];

#endif
