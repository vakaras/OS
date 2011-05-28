[GLOBAL msg]

msg:
dd 0xdead2220, 0xdead2222, 0xdead2222, 0xdead2223

info:  db "File content:",0xa,0

[GLOBAL _start]

_start:

  mov rbx, 0xdead3333
  xchg bx, bx

.loop:

  ; Išveda naudotojui informacinę žinutę.
  mov rcx, info
  mov rbx, 1
  call print

  ; Atsidaro 0x20 failą skaitymui.
  mov rax, 1
  mov rbx, 0x20
  int 0x3f

  ; Nusiskaito failo turinį.
  mov rbx, rax
  call read

  ; Uždaro atidarytąjį failą.
  mov rax, 3
  int 0x3f

  ; Išveda naudotojui failo turinį.
  mov rcx, buffer
  mov rbx, 1
  call print

  ; Išsijungia.
  mov rax, 0
  int 0x3f

print:
  mov rax, 6
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
  mov rcx, buffer
.loop:
  mov rax, 5
  int 0x3f
  cmp al, 0
  jz  .end
  mov [rcx], al
  inc rcx
  jmp .loop
.end:
  mov [rcx], byte 0
  ret

section .bss

buffer:   resb    1024
