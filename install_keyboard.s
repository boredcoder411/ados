.code16
.global install_keyboard
.type install_keyboard, @function

install_keyboard:
  push $0x00
  pop %ds
  cli
  movw $keyboard_isr, (4 * KEYBOARD_INTERRUPT)      # Store offset of ISR
  movw %cs, (4 * KEYBOARD_INTERRUPT + 2)            # Store segment of ISR
  sti
  ret

keyboard_isr:
  pusha

  in $0x60, %al
  test $0x80, %al
  jnz .keyboard_end

  mov $0x0e, %ah
  mov $0x61, %al
  int $0x10

.keyboard_end:
  mov $0x61, %al
  out %al, $0x20
  popa
  iret

.equ KEYBOARD_INTERRUPT, 9

