PROGS := trace-foo.c
COMMON_OBJS := foo.o

# flags for the linker
LDFLAGS = --wrap=foo  

# bootloader to use.
BOOTLOADER = pi-install

# automatically run the .bin files
RUN=1

# set this if you need to give the device for your pi
# TTYUSB = 

include $(CS140E_2023_PATH)/libpi/mk/Makefile.template
