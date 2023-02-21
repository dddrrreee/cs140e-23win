BOOTLOADER = my-install

PROGS := 2-test-fault.c 1-test-setup.c
PROGS := 1-test-setup.c
PROGS := $(wildcard ./[0-7]-test*.c)
PROGS := 0-test-structs.c

# make a local copy of cstart so you can modify it.
COMMON_SRC = mmu-helpers.c mmu.c vm-ident.c

# make sure can drop in staff.
STAFF_OBJS += staff-mmu-asm.o  
STAFF_OBJS += staff-mmu.o  

# set RUN = 1 if you want the code to automatically run after building.
RUN = 1

include $(CS140E_2023_PATH)/libpi/mk/Makefile.template
