[BITS 64]

[GLOBAL _start]
[EXTERN main]
[EXTERN __cxa_finalize]

extern start_ctors, end_ctors, start_dtors, end_dtors

_start:

  mov rax, 0xbabadeadbabadead           ; Pakeičiam registro reikšmę, kad
                                        ; žinotume, jog šitas kodas buvo
                                        ; įvykdytas.

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

  ; Kviečiamas C++ branduolys.
  cli                                   ; Išjungiame išorinius pertraukimus.
  call main                             ; Iškviečiame C++ branduolį.


  ; Iškviečiami visų globalių ir statinių objektų destruktoriai.
  ; FIXME: Susitvarkyti su kintamųjų dydžiais.
  sub esp, 4                            
  mov [esp], dword 0x0
  call __cxa_finalize
  add esp, 4

  jmp $
