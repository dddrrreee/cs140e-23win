# makefile configuration
# define this if you need to give the device for your pi
TTYUSB = 
# can override the default bootloader
BOOTLOADER = my-install

# the string to extract for checking
GREP_STR := 'HASH:\|ERROR:\|PANIC:\|SUCCESS:'

# set if you want the code to automatically run after building.
RUN = 1
# set if you want the code to automatically check after building.
#CHECK = 1

TEST_SRC := 2-test-fault.c 1-test-setup.c
TEST_SRC := 1-test-setup.c
TEST_SRC := $(wildcard ./[0-7]-test*.c)
TEST_SRC := 0-test-structs.c 

TEST_OUTS   := $(patsubst %.c, %.out, $(TEST_SRC))

# put all your miscellaneous assembly in this file. 
SUPPORT_OBJS = mmu-helpers.o mmu.o vm-ident.o 

# these are the binaries we give you: remove these when finished.
STAFF_OBJS =  staff-mmu.o staff-mmu-asm.o

include $(CS140E_2022_PATH)/libpi/mk/Makefile.template

cksum: 
	cksum $(TEST_OUTS) | sort -n | cksum
