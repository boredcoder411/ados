# Variables
CC = gcc
LD = ld
OBJCOPY = objcopy
CFLAGS = -std=c99 -c -Os -march=i686 -m32 -ffreestanding -fdata-sections -ffunction-sections \
         -fno-asynchronous-unwind-tables -fno-stack-protector -fomit-frame-pointer -Wall -Werror -nostartfiles
LDFLAGS = -static -T link.ld -m elf_i386 -nostdlib --nmagic
ASMFLAGS = -c -march=i686 -m32
NASM = nasm

# Targets and dependencies
all: clean kernel mbr

kernel: kernel.bin

kernel.bin: main.o fast.o link.ld
	$(LD) $(LDFLAGS) -o main.elf main.o fast.o
	$(OBJCOPY) -O binary main.elf kernel.bin

main.o: main.c
	$(CC) $(CFLAGS) main.c -o main.o

fast.o: fast.s
	$(CC) $(ASMFLAGS) fast.s -o fast.o

mbr: bootsector.bin

bootsector.bin: bootsector.s
	$(NASM) -f bin bootsector.s -o bootsector.bin

clean:
	rm -f main.o fast.o main.elf kernel.bin bootsector.bin

.PHONY: all clean

