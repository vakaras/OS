; This file content was copied from JohnnyTheDon post at 
; http://f.osdev.org/viewtopic.php?f=1&t=17548

%define LOAD_ADDR 01000h ; address this code is loaded at
ORG LOAD_ADDR

BITS 32

%define PNP_BUFFER_LOCATION LOAD_ADDR + 01000h

entry:
   mov dword [return_loc], edx ; save return location
   lgdt [gdtr] ; load gdt
   jmp 08h:do_pmode16_jmp
do_pmode16_jmp:
   jmp 018h:pmode16 ; switch to 16-bit pmode with a far call
BITS 16
pmode16:
   mov ax, 020h
   mov ss, ax ; load the stack segment with a 16-bit segment
   mov eax, cr0
   and eax, 0FFFFFFFEh
   mov cr0, eax ; clear the protected mode (PE) bit
   jmp 0h:realmode ; switch to real mode with a far call
realmode:
   mov ax, cs ; set all segments to the code segment
   mov ds, ax
   mov ss, ax
   mov es, ax
pmode32: ; switch to real mode
   lgdt [gdtr]
   mov eax, cr0
   or al, 1
   mov cr0, eax ; set protected mode (PE) bit
   jmp 018h:return_pmode16 ; jump into 32-bit code
BITS 16
return_pmode16:
   jmp 08h:return
BITS 32
return: ; return to the rest of double
   mov eax, 010h
   mov ds, eax
   mov ss, eax
   mov es, eax
   mov edx, dword [return_loc]
   mov dword [rewritable_jmp+1], edx ; rewrite the jump to the return location
rewritable_jmp:
   jmp 08h:0FFFFFFFh ; won't actually jump to this addr, this value is overwritten
return_loc dd 0

gdtr:
dw (_end_gdt-gdt)-1
dd gdt

gdt:
dq 0 ; null segment

dw 0FFFFh ; 32-bit code segment
dw 0
db 0
db 10011010b
db 11001111b
db 0

dw 0FFFFh ; 32-bit data segment
dw 0
db 0
db 10010010b
db 11001111b
db 0

dw 0FFFFh ; 16-bit code segment
dw 0
db 0
db 10011010b
db 00001111b
db 0

dw 0FFFFh ; 16-bit data segment
dw 0
db 0
db 10010010b
db 00001111b
db 0
_end_gdt:
