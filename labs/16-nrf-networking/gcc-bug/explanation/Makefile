.PHONY: all clean

all: example.list

%.list: %.o
	arm-none-eabi-objdump -d $< > $@

%.o: %.c
	arm-none-eabi-gcc -c $< -mcpu=arm1176jzf-s -O3 -o $@

clean:
	rm -rf *.o *.list
