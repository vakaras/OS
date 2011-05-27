[GLOBAL msg]

msg:
dd 0xdeadbbb0, 0xdeadbbb1, 0xdeadbbb2, 0xdeadbbb3

[GLOBAL _start]

_start:

  mov rbx, 0xdeadbbbb

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
