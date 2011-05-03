; Sustabdo procesorių aktyvuodama bochs derintuvę.
global pause, pause0, pause1, pause2 
pause2:
pause1:
pause0:
pause:
  xchg bx, bx
  ret
