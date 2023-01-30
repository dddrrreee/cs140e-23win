# need the makefile to give an error if this doesn't exist
PROGS := main.c
COMMON_SRC := get-code.c

START := boot-start.o

# uncomment if you want it to automatically run.
# RUN=1

# define this if you need to give the device for your pi
TTYUSB = 
BOOTLOADER = pi-install

include $(CS140E_2023_PATH)/libpi/mk/Makefile.template

all:: kernel.img

clean::
	rm -f $(START) kernel.img

kernel.img: main.bin
	cp main.bin kernel.img
