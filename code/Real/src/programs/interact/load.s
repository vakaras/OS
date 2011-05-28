[BITS 64]

[GLOBAL _start]
[EXTERN main]
[EXTERN __cxa_finalize]

extern start_ctors, end_ctors, start_dtors, end_dtors

_start:

  ; Iškviečiami statinių ir globalių objektų konstruktoriai.
static_ctors_loop:
  mov rbx, start_ctors
  jmp .test
.body:
  call [rbx]
  add rbx, 8
.test:
  mov rdx, end_ctors
  cmp rbx, rdx
  jb .body

  call main

  mov rax, 0
  int 0x3f
