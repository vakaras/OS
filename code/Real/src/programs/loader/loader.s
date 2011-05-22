[GLOBAL msg]

msg:
dd 0xdeadeee0, 0xdeadeee1, 0xdeadeee2, 0xdeadeee3

[GLOBAL _start]

_start:

  mov rbx, 0xdeadeeee
  xchg bx, bx

.loop:
  
  ; Gaunam užduotį.
  mov rax, 10
  int 0x3f
  mov rdi, rax
  mov rsi, rbx
  xchg bx, bx

  ; Gaunam atmintį.
  mov rax, 11
  int 0x3f
  mov rdx, rax
  xchg bx, bx
  
  ; Kuriam procesą.
  mov rax, 12
  int 0x3f

  jmp .loop
