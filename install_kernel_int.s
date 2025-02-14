.code16
.global install_kernel_int
.type install_kernel_int, @function

install_kernel_int:
  push $0x00
  pop %ds
  cli
  movw $kernel_isr, (4 * KERNEL_INTERRUPT)      # Store offset of ISR
  movw %cs, (4 * KERNEL_INTERRUPT + 2)            # Store segment of ISR
  sti
  ret

kernel_isr:
  pusha

  in $0x60, %al
  test $0x80, %al
  jnz .kernel_end

  mov $0x0e, %ah
  mov $0x61, %al
  int $0x10

.kernel_end:
  mov $0x61, %al
  out %al, $0x20
  popa
  iret

.equ KERNEL_INTERRUPT, 21

