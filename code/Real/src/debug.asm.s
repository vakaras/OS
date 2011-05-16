; Sustabdo procesorių aktyvuodama bochs derintuvę.
global pause, pause0, pause1, pause2 
pause2:
pause1:
pause0:
pause:
  xchg bx, bx
  ret

; Perduoda vykdymą į nurodytą adresą.
global jump

jump:
  xchg bx, bx
  jmp rdi
