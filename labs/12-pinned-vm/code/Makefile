BOOTLOADER = my-install

PROGS += tests/1-test-setup.c
PROGS += tests/1-test-two-addr.c
PROGS += tests/2-test-procmap.c
PROGS += tests/2-test-lookup.c

# make a local copy of cstart so you can modify it.
COMMON_SRC = pinned-vm.c

# make sure can drop in staff.
STAFF_OBJS += staff-pinned-vm.o
STAFF_OBJS += staff-mmu-asm.o  
STAFF_OBJS += staff-mmu-except.o  
STAFF_OBJS += staff-mmu.o  


# define this if you need to give the device for your pi
TTYUSB = 

# set RUN = 1 if you want the code to automatically run after building.
RUN = 1

include $(CS140E_2023_PATH)/libpi/mk/Makefile.template
