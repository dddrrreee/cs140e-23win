# need the makefile to give an error if this doesn't exist
PROGS := timer.c
COMMON_SRC := interrupts-c.c	timer-interrupt.c interrupts-asm.S
# SUPPORT_OBJS := interrupts-c.c	timer-interrupt.c


# define this if you need to give the device for your pi
TTYUSB = 
BOOTLOADER = my-install

# uncomment if you want it to automatically run.
RUN=1


GREP_STR := 'TRACE:\|THREAD:\|ERROR:\|PANIC:' 

# CFLAGS_EXTRA =  -fno-omit-frame-pointer -g -funwind-tables -fno-exceptions
CFLAGS_EXTRA =  -fno-omit-frame-pointer -mpoke-function-name

include $(CS140E_2023_PATH)/libpi/mk/Makefile.template
