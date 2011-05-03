;
; Įvairūs primityvai.
;

; Pagalbinės funkcijos derinimui.
[EXTERN debug_ping]
[EXTERN debug_char]


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

idt_flush:
  lidt [rdi]                            ; Atnaujinama rodyklė į IDT lentelę.
  ret


; Puslapiavimo aktyvavimas.
; void load_pager(u64int);
[GLOBAL load_pager]

load_pager:
  mov cr3, rdi
  jmp $
  ret
