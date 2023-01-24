# change from bug1 to bug2 to bug3 to bug4
BUG ?= bug2
PROGS := $(BUG)-driver.c
COMMON_SRC := $(BUG)-asm.S

# define this if you need to give the device for your pi
TTYUSB = 
BOOTLOADER = pi-install

# uncomment if you want it to automatically run.
RUN ?= 1

GREP_STR := 'TRACE:\|THREAD:\|ERROR:\|PANIC:' 

include $(CS140E_2023_PATH)/libpi/mk/Makefile.template


# ugly --- built-in libpi/mk/Makefile.template does not let you
# have program-specific files per program
makeall:
	make -C . "BUG=bug1" "RUN=0"
	make -C . "BUG=bug2" "RUN=0"
	make -C . "BUG=bug3" "RUN=0"
	make -C . "BUG=bug4" "RUN=0"

cleanall::
	make -C . "BUG=bug1" clean
	make -C . "BUG=bug2" clean
	make -C . "BUG=bug3" clean
	make -C . "BUG=bug4" clean

.PHONEY: cleanall makeall
