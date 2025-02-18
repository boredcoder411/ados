[org 0x7c00]
[bits 16]

mov [BOOT_DRIVE], dl

xor ax, ax
mov ds, ax
mov es, ax
mov ss, ax
mov fs, ax
mov gs, ax

cld

mov bp, 0x7c00
mov sp, bp

mov ah, 0x00
mov al, 0x03
int 0x10

;; Load the kernel from disk
mov bx, 0x8000
mov dh, NUM_KERNEL_SECTORS
mov dl, [BOOT_DRIVE]
call disk_load
push dx
call 0x8000

disk_load:
  pusha
  push dx
  mov ah, 0x02
  mov al, dh
  mov ch, 0x00
  mov dh, 0x00
  mov cl, 0x02
  int 0x13
  pop dx
  popa
  ret

BOOT_DRIVE db 0

times 512-($-$$) db 0

NUM_KERNEL_SECTORS equ (kernel_end - kernel_start + 511) / 512

kernel_start:
  incbin "kernel.bin"
kernel_end:
