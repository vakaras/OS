[BITS 64]

[GLOBAL _start]

_start:

  mov rax, 0xbabadeadbabadead           ; Pakeičiam registro reikšmę, kad
                                        ; žinotume, jog šitas kodas buvo
                                        ; įvykdytas ir įeiname į begalinį
                                        ; ciklą.
  jmp $
