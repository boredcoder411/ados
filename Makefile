all:
	gcc -std=c99 -c -Os -march=i686 -m32 -ffreestanding -fdata-sections -ffunction-sections -fno-asynchronous-unwind-tables -fno-stack-protector -fomit-frame-pointer -Wl,--gc-sections -Wall -Werror main.c -o main.o
	ld -static -T link.ld -m elf_i386 -nostdlib --nmagic -o main.elf main.o
	objcopy -O binary main.elf main.bin

clean:
	rm -f main.o main.elf main.bin
