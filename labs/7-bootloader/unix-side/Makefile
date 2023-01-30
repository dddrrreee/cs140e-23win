CC = gcc
SRC = my-install.c simple-boot.c 
OBJS = $(SRC:.c=.o)
PI_SIDE = ../pi-side

# check that your path is defined.
ifndef CS140E_2023_PATH
$(error CS140E_2023_PATH is not set: this should contain the absolute path to where this directory is.  Define it in your shell's initialiation.  For example, .tcshrc for tcsh or .bashrc for bash)
endif

PATH_LIBUNIX = $(CS140E_2023_PATH)/libunix
LIBUNIX = $(PATH_LIBUNIX)/libunix.a
CFLAGS = -Wall -Werror -g  -I$(PATH_LIBUNIX) -I$(PI_SIDE) -Wno-unused-function -I.

# massively over depend to keep things simple.
DEPS := $(wildcard ./*.c) $(LIBUNIX) $(wildcard ./*.h $(PI_SIDE)/*.h) ./Makefile 

# uncomment:
#   - the 'run' target to automatically run the hello.bin code.
#   - the 'tags' target to run tags on the code.  this is super
#     helpful.
all : libunix my-install # run # tags 

libunix:
	make -C $(PATH_LIBUNIX)

$(OBJS): $(DEPS)

my-install:  $(OBJS) $(DEPS)
	$(CC) $(CFLAGS) $(OBJS) -o my-install -lm $(LIBUNIX)

tags:
	ctags *.[ch]

run:
	./my-install ./hello.bin


clean :
	rm -f *.o *~ Makefile.bak my-install  tags
	make -C $(PATH_LIBUNIX) clean

.PHONY: run clean tags libunix
