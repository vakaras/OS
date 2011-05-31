[GLOBAL pause2]

pause2:
  xchg bx, bx
  ret


[GLOBAL open_file_read]

open_file_read:
  push rbx
  mov rax, 1
  mov rbx, rdi
  int 0x3f
  pop rbx
  ret

[GLOBAL open_file_write]

open_file_write:
  push rbx
  mov rax, 2
  mov rbx, rdi
  int 0x3f
  pop rbx
  ret

[GLOBAL close_file]

close_file:
  push rbx
  mov rax, 3
  mov rbx, rdi
  int 0x3f
  pop rbx
  ret

[GLOBAL read_byte]

read_byte:
  push rbx
  mov rbx, rdi
  mov rax, 5
  int 0x3f
  pop rbx
  ret

[GLOBAL write_byte]

write_byte:
  push rbx
  mov rbx, rsi
  mov rax, 6
  int 0x3f
  pop rbx
  ret
