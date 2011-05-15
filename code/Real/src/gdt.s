global load_gdt

load_gdt:
  mov rax, qword gdtr
  lgdt [rax]
  ret

gdtr:
dw (_end_gdt-gdt)-1
dq gdt
gdt:
dq 0 ; define null segment

dd 0 ; 64-bit code segment
db 0
db 10011010b
db 10100000b
db 0

dd 0 ; 64-bit data segment
db 0
db 10010010b
db 10100000b
db 0

_end_gdt:
