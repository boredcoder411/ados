# Variables
CC = gcc
LD = ld
OBJCOPY = objcopy
CFLAGS = -std=c99 -c -Os -march=i686 -m32 -ffreestanding -fdata-sections -ffunction-sections \
         -fno-asynchronous-unwind-tables -fno-stack-protector -fomit-frame-pointer -Wall -Werror -nostartfiles
LDFLAGS = -static -T link.ld -m elf_i386 -nostdlib --nmagic
ASMFLAGS = -c -march=i686 -m32
NASM = nasm

# Automatically find all .c and .s files
C_SOURCES := $(wildcard *.c)
ASM_SOURCES := $(wildcard *.s)
OBJ_FILES := $(C_SOURCES:.c=.o) $(ASM_SOURCES:.s=.o)

# Targets and dependencies
all: clean kernel mbr

kernel: kernel.bin

kernel.bin: $(OBJ_FILES) link.ld
	$(LD) $(LDFLAGS) -o main.elf $(OBJ_FILES)
	$(OBJCOPY) -O binary main.elf kernel.bin

%.o: %.c
	$(CC) $(CFLAGS) $< -o $@

%.o: %.s
	$(CC) $(ASMFLAGS) $< -o $@

mbr: bootsector.bin

bootsector.bin: bootsector.asm
	$(NASM) -f bin bootsector.asm -o bootsector.bin

clean:
	rm -f $(OBJ_FILES) main.elf kernel.bin bootsector.bin image.img

.PHONY: all clean

