# need the makefile to give an error if this doesn't exist
PROGS := hello.c
COMMON_SRC := trace.c

# how do we redo libpi?
CFLAGS +=  -finstrument-functions 

# uncomment if you want it to automatically run.
# RUN=1

# define this if you need to give the device for your pi
TTYUSB = 
BOOTLOADER = pi-install

include $(CS140E_2023_PATH)/libpi/mk/Makefile.template
