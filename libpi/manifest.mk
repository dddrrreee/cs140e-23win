BUILD_DIR := ./objs
LIB := libpi.a
START := ./staff-start.o

# set this to the path of your ttyusb device if it can't find it
# automatically
TTYUSB = 

# the string to extract for checking
GREP_STR := 'HASH:\|ERROR:\|PANIC:\|PASS:\|TEST:'

# set if you want the code to automatically run after building.
RUN = 1
# set if you want the code to automatically check after building.
#CHECK = 0

ifdef CS140E_STAFF
ifndef CS140_ACT_AS_STUDENT
STAFF_OBJS += staff-objs/kmalloc.o
#STAFF_OBJS += staff-objs/sw-uart.o
#STAFF_OBJS += staff-objs/interrupts-asm.o      
# STAFF_OBJS += staff-objs/interrupts-vec-asm.o 
#STAFF_OBJS += staff-objs/rpi-thread.o 
#STAFF_OBJS += staff-objs/rpi-thread-asm.o 
#STAFF_OBJS += staff-objs/new-spi.o
endif
endif

DEPS += ./src


include $(CS140E_2023_PATH)/libpi/mk/Makefile.lib.template

all:: $(START)

staff-start.o: ./objs/staff-start.o
	cp ./objs/staff-start.o staff-start.o

clean::
	rm -f staff-start.o
	rm -f staff-start-fp.o
	make -C  libc clean
	make -C  staff-src clean

.PHONY : libm test
