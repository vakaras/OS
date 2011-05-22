[GLOBAL msg]

msg:
dd 0xdeadbab0, 0xdeadbab1, 0xdeadbab2, 0xdeadbab3

[GLOBAL _start]

_start:

  mov rbx, 0xdead000a
  xchg bx, bx

.loop:
  
  ; Išveda derinimo pranešimą.
  mov al, 'a'
  out 0xe9, al

  ; Prašo resurso.
  mov rax, 21
  int 0x3f

  ; Kuria resursą.
  mov rax, 20
  int 0x3f

  jmp .loop
