BOOTLOADER=my-install

START = staff-start.o 

PROGS := $(wildcard tests/0-*.c)
PROGS := $(wildcard tests/1-*.c)
PROGS := tests/0-prog-alloc.c
PROGS := tests/3-do-all.c

# have a common library for the tests
# LIB = libasm.a 
LIB_POST = ./libgcc.a

COMMON_SRC += equiv-os.c
COMMON_SRC += sec-alloc.c
COMMON_SRC += support-code.c

# CFLAGS = -I.


########################################################################
# Part 0 [you might want to do this later]
#   this has:
#     <switchto_user_asm> 
#   and
#     <equiv_full_vec> vector that handles:
#           <data_abort_full>, 
#           <prefetch abort_full>
#           <syscall_full>
# from lab 11
STAFF_OBJS += staff-equiv-os-asm.o

########################################################################
# Part 1:
#    this is from lab 11.
SS_LAB = 11-equiv-proc/code
STAFF_OBJS += staff-breakpoint.o

# will have to uncomment both
# CFLAGS += -I$(SS_LAB)
# COMMON_SRC += $(SS_LAB)/breakpoint.c

########################################################################
# Part 2: 
#    this is from lab 12.
PIN_LAB = ../../12-pinned-vm/code
CFLAGS += -I$(PIN_LAB)
STAFF_OBJS += staff-pinned-vm.o
# COMMON_SRC += $(PIN_LAB)/pinned-vm.c

########################################################################
# Part 3:
#   this is from lab 13
VM_LAB = ../../13-vm-page-table/code
STAFF_OBJS += staff-mmu-asm.o
# uncomment both to swap
# CFLAGS += -I$(VM_LAB)
# COMMON_SRC += $(VM_LAB)/your-mmu-asm.S

# this is from lab 13
STAFF_OBJS += staff-mmu.o
# COMMON_SRC += $(VM_LAB)/mmu.c

########################################################################

# automatically run.
RUN=1
MEMMAP = ./memmap

CAN_EMIT=1

EXCLUDE ?= grep -v simple_boot
include $(CS140E_2023_PATH)/libpi/mk/Makefile.template-v2

all:: # prog-gen

#libs::
#	make -C user-progs

#user-progs/byte-array-prog-vec.h:
#	make -C user-progs
#user-progs/byte-array-prog-exit.h:
#	make -C user-progs
#user-progs/byte-array-prog-alloc.h:
#	make -C user-progs

prog-gen:
	make -C user-progs

clean::
	rm -f staff-start.o
	make -C user-progs clean
	make -C tests clean

.PHONEY: prog-gen
