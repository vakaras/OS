[GLOBAL msg]

msg:
dd 0xdead1110, 0xdead1111, 0xdead1112, 0xdead1113

[GLOBAL _start]

_start:

  mov rbx, 0xdead1111
  xchg bx, bx

  ; Paprašo, kad išjungtų.
  mov rax, 0
  int 0x3f
