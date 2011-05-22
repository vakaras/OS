global switch_process

switch_process:

  mov rsp, rdi

  ;xchg bx, bx

  add rsp, 8                            ; Išmetam pertraukimo numerį.

  pop r15
  pop r14
  pop r13
  pop r12
  pop r11
  pop r10
  pop r9
  pop r8

  pop rdi
  pop rsi
  pop rbp
  pop rbx
  pop rdx
  pop rcx
  pop rax

  add rsp, 8                            ; Išmetam klaidos kodą.

  iretq
