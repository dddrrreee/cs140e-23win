PROGS := prog-vec.c

PROG_HEADERS := $(patsubst %.c, byte-array-%.h, $(PROGS))

# use our own custom cstart
COMMON_OBJS := loader-cstart.o
# override memmap
MEMMAP = ./memmap.fixed
# override start 
START = loader-start.o

# define this if you need to give the device for your pi
TTYUSB = 
BOOTLOADER = my-install

# uncomment if you want it to automatically run.
RUN=0
CFLAGS += -I..
COMMON_SRC += ../libos-asm.S

include $(CS140E_2023_PATH)/libpi/mk/Makefile.template-v2

all:: code-gen $(PROG_HEADERS)

byte-array-%.h: %.bin code-gen
	code-gen $< > byte-array-$*.h

# add more actions to clean
clean::
	rm -f *.o code-gen $(PROG_HEADERS)

code-gen:  code-gen.c
	make -f Makefile.unix
