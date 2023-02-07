ALL= $(wildcard [01]-test*.c)

all:
	make -f ./Makefile PROGS="$(ALL)" clean
	make -f ./Makefile PROGS="$(ALL)" gen-binaries
	make -f ./Makefile PROGS="$(ALL)" check
	@cksum *.out | sort -n | cksum

#cleanall:
#	rm *.o *.elf *.bin *.list
