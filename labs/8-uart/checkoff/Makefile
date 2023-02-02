PROG_SRC = hash-file.c

# CS140E_USE_STAFF = 1
include $(CS140E_2023_PATH)/libunix/mk/Makefile.unix

all: check

# so you don't have to type as much
# you can add whatever.
check: ./hash-file
	@echo "-------------------------------------------------------------";
	@echo "running pi code";
	my-install hash-bootloader.bin
	@echo "-------------------------------------------------------------";
	@echo "unix side code";
	./hash-file  ../../7-bootloader/2-pi-side/kernel.img
