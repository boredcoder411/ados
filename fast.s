  .globl HALT
  .type HALT, @function
HALT:
  cli
  jmp HALT
