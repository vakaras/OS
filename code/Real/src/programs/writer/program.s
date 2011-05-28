[GLOBAL msg]

msg:
dd 0xdead2220, 0xdead2222, 0xdead2222, 0xdead2223

ask_content:  db "Enter file content (one line):",0xa,0

[GLOBAL _start]

_start:

  mov rbx, 0xdead3333
  xchg bx, bx

.loop:

  ; Paprašo naudotojo įvesti eilutę.
  mov rcx, ask_content
  mov rbx, 1
  call print

  ; Nusiskaito įvestąją eilutę.
  call read

  ; Atsidaro (sukuria?) 0x20 failą rašymui.
  mov rax, 2
  mov rbx, 0x20
  int 0x3f

  ; Įrašo kvailą žinutę į atidarytąjį failą.
  mov rbx, rax
  mov rcx, ask_content
  call print

  ; Uždaro atidarytąjį failą.
  mov rax, 3
  int 0x3f

  ; Atidaro 0x20 failą rašymui.
  mov rax, 2
  mov rbx, 0x20
  int 0x3f

  ; Įrašo naudotojo žinutę.
  mov rbx, rax
  mov rcx, buffer
  call print

  ; Išsijungia neuždaręs failo.
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
