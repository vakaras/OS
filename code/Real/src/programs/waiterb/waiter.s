[GLOBAL msg]

msg:
dd 0xdeadbab0, 0xdeadbab1, 0xdeadbab2, 0xdeadbab3

[GLOBAL _start]

_start:

  mov rbx, 0xdead000b
  xchg bx, bx

.loop:
  
  ; Išveda derinimo pranešimą.
  mov al, 'b'
  out 0xe9, al

  ; Kuria resursą.
  mov rax, 30
  int 0x3f

  ; Prašo resurso.
  mov rax, 31
  int 0x3f

  jmp .loop
