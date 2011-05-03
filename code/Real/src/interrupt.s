extern interrupt_handler


align 16

%assign i 0
%rep 64

global isr %+ i
isr %+ i:
  
  cli

%if i <= 7
  push qword 0
%endif
%if i == 9
  push qword 0
%endif
%if i == 16
  push qword 0
%endif
%if i >= 18
  push qword 0
%endif

  push qword i
  jmp isr_common_stub

%assign i i + 1
%endrep

temp dq 0

; Visiems pertraukimams bendras apdorojimo kodas.
isr_common_stub:

  ;mov qword [Temp], rsp
  push rsp
  push rax
  push rcx
  push rdx
  push rbx
  ;push QWORD [temp]
  push rbp
  push rsi
  push rdi

  push r8
  push r9
  push r10
  push r11
  push r12
  push r13
  push r14
  push r15

  mov rdi, rsp

  call interrupt_handler

  ; TODO: Užbaigti pagal arcticos/arcticos-read-only/Core/isr.asm
