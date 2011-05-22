[GLOBAL msg]

msg:
dd 0xdeadbab0, 0xdeadbab1, 0xdeadbab2, 0xdeadbab3

[GLOBAL _start]

_start:

  mov rbx, 0xdeadbaba
  xchg bx, bx
.loop:
  mov al, 'v'
  ;out 0xe9, al
  ;mov rax, 0
  ;int 0x3f
  jmp .loop
