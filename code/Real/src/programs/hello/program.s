[GLOBAL msg]

msg:
dd 0xdead2220, 0xdead2222, 0xdead2222, 0xdead2223

ask_name:  db "Koks tavo vardas?",0xa,0
say_hello: db "Labas, ",0

[GLOBAL _start]

_start:

  mov rbx, 0xdead2222
  xchg bx, bx

.loop:

  mov rcx, ask_name
  call print

  call read

  mov rcx, say_hello
  call print
  mov rcx, buffer
  call print
  mov rax, 6
  mov rbx, 1
  mov rdi, '!'
  int 0x3f
  mov rdi, 10
  int 0x3f

  mov rax, 0
  int 0x3f

print:
  mov rax, 6
  mov rbx, 1
.loop:
  mov dl, [rcx]
  cmp dl, 0
  jz  .end
  mov rdi, rdx
  int 0x3f
  inc rcx
  jmp .loop
.end:
  ret

read:
  mov rbx, 0
  mov rcx, buffer
.loop:
  mov rax, 5
  int 0x3f
  cmp al, 10
  jz  .end
  mov [rcx], al
  inc rcx
  jmp .loop
.end:
  mov [rcx], byte 0
  ret

section .bss

buffer:   resb    1024
