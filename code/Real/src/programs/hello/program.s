[GLOBAL msg]

msg:
dd 0xdead1110, 0xdead1111, 0xdead1112, 0xdead1113

[GLOBAL _start]

_start:

  mov rbx, 0xdead1111
  xchg bx, bx

.loop:

  mov rbx, 0
  mov rax, 5
  int 0x3f

  mov rdi, rax
  mov rbx, 1
  mov rax, 6
  int 0x3f

  jmp .loop
