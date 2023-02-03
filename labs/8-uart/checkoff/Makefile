PROG_SRC = hash-file.c




BOOT_DIR = ../../7-bootloader/2-pi-side/

#CS140E_USE_STAFF = 1
#BOOT_DIR = ../../7-bootloader/2-pi-side.staff/

include $(CS140E_2023_PATH)/libunix/mk/Makefile.unix

all: check-boot-hash

checkoff:
	make -C ./ check-boot-hash
	
	make -C ../ clean
	make -C ../1-uart checkoff

	make -C ../ clean
	make -C ../3-sw-uart checkoff
	
	make -C ../../7-bootloader/ clean
	make -C ../../7-bootloader/checkoff checkoff
	
# so you don't have to type as much
# you can add whatever.
check-boot-hash: ./hash-file
	@echo "-------------------------------------------------------------";
	@echo "running pi code";
	my-install hash-bootloader.bin
	@echo "-------------------------------------------------------------";
	@echo "unix side code";
	./hash-file  $(BOOT_DIR)/kernel.img
	arm-none-eabi-nm $(BOOT_DIR)/main.elf | grep using_staff_uart

