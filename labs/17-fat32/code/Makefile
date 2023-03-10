# uncomment COMMON_SRC and comment out STAFF_OBJS
# COMMON_SRC += mbr.c
STAFF_OBJS +=  staff-mbr.o

# uncomment COMMON_SRC and comment out STAFF_OBJS
# COMMON_SRC += fat32.c
STAFF_OBJS +=  staff-fat32.o

# the tests in decreasing order of difficulty.
PROGS := $(wildcard ./tests/[2]-*.c)
PROGS := $(wildcard ./tests/[1]-*.c)
PROGS := $(wildcard ./tests/[0]-*.c)

CFLAGS_EXTRA  = -Iexternal-code

# a list of all of your object files.
COMMON_SRC += pi-sd.c mbr-helpers.c fat32-helpers.c fat32-lfn-helpers.c external-code/unicode-utf8.c external-code/emmc.c external-code/mbox.c 

# external-code/bzt-sd.c 

TTYUSB = 
BOOTLOADER = my-install
RUN = 1

EXCLUDE ?= grep -v simple_boot
GREP_STR := 'HASH:\|ERROR:\|PANIC:\|PASS:\|TEST:'
include $(CS140E_2023_PATH)/libpi/mk/Makefile.template-v2

clean::
	rm -f tests/*.list tests/*.o tests/*.elf tests/*~ tests/*.bin
	make -C external-code clean
