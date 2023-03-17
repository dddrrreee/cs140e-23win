PROGS := $(wildcard prog-*.c)

PROG_HEADERS := $(patsubst %.c, byte-array-%.h, $(PROGS))

# override memmap
MEMMAP = ./memmap.fixed

# set libpi to nothing.
LPI = 

# override start 
START = loader-start.o

LP = $(CS140E_2023_PATH)/libpi

CFLAGS += -I..
COMMON_SRC += ../libos-asm.S
COMMON_SRC += ../libos.c
COMMON_SRC +=  loader-cstart.c
COMMON_SRC += $(LP)/libc/memset.c

RUN=0 
BOOTLOADER = my-install
include $(CS140E_2023_PATH)/libpi/mk/Makefile.template-v2

all:: code-gen $(PROGS:.c=.bin) $(PROG_HEADERS)

byte-array-%.h: %.bin code-gen
	./code-gen $< > byte-array-$*.h

# add more actions to clean
clean::
	rm -f *.o code-gen ../libos-asm.o ../libos.o
# $(PROG_HEADERS) 

code-gen:  code-gen.c
	make -f Makefile.unix
