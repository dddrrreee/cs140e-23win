TESTS = $(wildcard ./*.bin)
TESTS = 0-ack-hw-check.bin   0-no-ack-hw-check.bin  1-one-way-ack.bin  1-one-way-no-ack.bin 3-loop-msg.bin

GREP_STR := 'HASH:\|ERROR:\|PANIC:\|SUCCESS:\|NRF'

BOOTLOADER = my-install
include $(CS140E_2023_PATH)/libpi/mk/Makefile.check
