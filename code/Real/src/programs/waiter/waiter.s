[GLOBAL msg]

msg:
dd 0xdeadbab0, 0xdeadbab1, 0xdeadbab2, 0xdeadbab3

[GLOBAL _start]

_start:

  mov rax, 0
  int 0x3f
  jmp $
