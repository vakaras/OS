[GLOBAL msg]

msg:
dd 0xdeadaaa0, 0xdeadaaa1, 0xdeadaaa2, 0xdeadaaa3

[GLOBAL _start]

_start:

  mov rbx, 0xdeadaaaa

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
