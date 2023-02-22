BOOTLOADER = my-install

PROGS := 2-test-fault.c 1-test-setup.c
PROGS := 0-test-structs.c

PROGS := 1-test-setup.c
PROGS := $(wildcard ./[0-7]-test*.c)


# just some helpers: you should look through, has printing etc 
# routines.
COMMON_SRC = mmu-helpers.c 

# part 1: get rid of staff-vm-ident.o
STAFF_OBJS += staff-vm-ident.o
#COMMON_SRC += vm-ident.c

# part 2: get rid of staff-mmu.o
STAFF_OBJS += staff-mmu.o  
COMMON_SRC += mmu.c 

STAFF_OBJS += staff-mmu-except.o

# <staff-mmu-asm.o> this will be removed next lab
STAFF_OBJS += staff-mmu-asm.o  
STAFF_OBJS += $(CS140E_2023_PATH)/libpi/staff-objs/interrupts-asm.o
STAFF_OBJS += $(CS140E_2023_PATH)/libpi/staff-objs/interrupts-vec-asm.o

# set RUN = 1 if you want the code to automatically run after building.
RUN = 1

GREP_STR := 'HASH:\|ERROR:\|PANIC:\|SUCCESS:'
include $(CS140E_2023_PATH)/libpi/mk/Makefile.template
