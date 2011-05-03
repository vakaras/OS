;
; Įvairūs primityvai.
;

; Pagalbinės funkcijos derinimui.
[EXTERN debug_ping]
[EXTERN debug_char]

; Sustabdo procesorių begaliniame cikle. (Funkcija derinimui.)
global pause, pause0, pause1, pause2 
pause2:
pause1:
pause0:
pause:
  xchg bx, bx
  ret

; GDT aktyvavimas.
; void gdt_flush(u64int);
[GLOBAL gdt_flush]

gdt_flush:

  lgdt [rdi]                            ; Atnaujinama rodyklė į GDT lentelę.

  ; Atnaujiname registrus, kad įsigaliotų pakeitimai.
  mov ax, 0x10      
  mov ds, ax
  mov es, ax
  mov fs, ax
  mov gs, ax
  mov ss, ax
  jmp .flush                            ; FIXME: Čia turėtų būti tolimas 
                                        ; šuolis…
.flush:
  ret

; IDT aktyvavimas.
; void idt_flush(u64int);
[GLOBAL idt_flush]

install_idt:
  lidt [rdi]                            ; Atnaujinama rodyklė į IDT lentelę.
  call debug_ping
  ret

; Puslapiavimo aktyvavimas.
; void load_pager(u64int);
[GLOBAL load_pager]

load_pager:
  mov cr3, rdi
  jmp $
