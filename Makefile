# Variables
CC = gcc
LD = ld
OBJCOPY = objcopy
NASM = nasm
CFLAGS = -std=c99 -c -Os -march=i686 -m32 -ffreestanding -fdata-sections -ffunction-sections \
         -fno-asynchronous-unwind-tables -fno-stack-protector -fomit-frame-pointer -Wall -Werror
LDFLAGS = -static -T link.ld -m elf_i386 -nostdlib --nmagic
ASMFLAGS = -f bin

# Targets and dependencies
all: kernel mbr

kernel: kernel.bin

kernel.bin: main.o link.ld
	$(LD) $(LDFLAGS) -o main.elf main.o
	$(OBJCOPY) -O binary main.elf kernel.bin

main.o: main.c
	$(CC) $(CFLAGS) main.c -o main.o

mbr: bootsector.bin

bootsector.bin: bootsector.s
	$(NASM) $(ASMFLAGS) bootsector.s -o bootsector.bin

clean:
	rm -f main.o main.elf kernel.bin bootsector.bin

.PHONY: all clean
