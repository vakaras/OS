[BITS 64]

[GLOBAL _start]
[EXTERN main]
[EXTERN __cxa_finalize]

[EXTERN debug_char]
[EXTERN debug_string]

msg_starting_cpp db   'Starting C++ code. Runing global and ',\
                      'static constructors.',10,0
msg_entering_main db  'Entering C++ main.',10,0
msg_returned_main db  'Returned from C++ main.',10,0

extern start_ctors, end_ctors, start_dtors, end_dtors

_start:

  mov rax, 0xbabadeadbabadead           ; Pakeičiam registro reikšmę, kad
                                        ; žinotume, jog šitas kodas buvo
                                        ; įvykdytas.

  mov rsp, 0x00000000010fa000           ; „Pasidedame“ dėklą į mums patogią 
                                        ; poziciją. (TODO: Išsiaiškinti,
                                        ; ar to pakanka.)

  ; Parodomas derinimo pranešimas.
  mov rdi, msg_starting_cpp
  call debug_string

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

  mov rdi, msg_entering_main
  call debug_string

  ; Kviečiamas C++ branduolys.
  cli                                   ; Išjungiame išorinius pertraukimus.
  call main                             ; Iškviečiame C++ branduolį.

  mov rdi, msg_returned_main
  call debug_string

  ; Iškviečiami visų globalių ir statinių objektų destruktoriai.
  ; FIXME: Susitvarkyti su kintamųjų dydžiais.
  sub esp, 4                            
  mov [esp], dword 0x0
  call __cxa_finalize
  add esp, 4

  jmp $
