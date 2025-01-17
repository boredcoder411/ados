[org 0x7c00]
[bits 16]

mov [BOOT_DRIVE], dl

mov bp, 0x7c00
mov sp, bp

;; Load the kernel from disk
mov bx, 0x8000
mov dh, NUM_KERNEL_SECTORS
mov dl, [BOOT_DRIVE]
call disk_load
jmp 0x8000

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

times 446-($-$$) db 0

db 0x80

times 510-($-$$) db 0
dw 0xaa55

NUM_KERNEL_SECTORS equ (kernel_end - kernel_start + 511) / 512

kernel_start:
  incbin "kernel.bin"
kernel_end:
