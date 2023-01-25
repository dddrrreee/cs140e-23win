# need the makefile to give an error if this doesn't exist
PROGS := gprof.c
# these are the same as in 0-timer-int
COMMON_SRC := interrupts-c.c	timer-interrupt.c interrupts-asm.S
STAFF_OBJS = staff-kmalloc.o

# uncomment if you want it to automatically run.
# RUN=1

# define this if you need to give the device for your pi
TTYUSB = 
BOOTLOADER = pi-install

include $(CS140E_2023_PATH)/libpi/mk/Makefile.template
