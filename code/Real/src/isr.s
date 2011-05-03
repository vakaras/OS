[BITS 64]

[EXTERN default_interrupt_handler]
[EXTERN debug_ping]

Temp dq 0
LazySSEStateSave    db  1

[GLOBAL isr_table]

align 16
isr_table:
%assign n 0
%rep 64
    dq isr %+ n
%assign n n + 1
%endrep
dq 'Ds'

align 16
%assign n 0
%rep 64
isr %+ n :              ; defines symbols isr0, isr1 etc.
push qword 0
cli
call debug_ping
sti
jmp $
;     cli
; %if n <= 7 
;     push qword 0        ; push a fake error code
; %endif
; %if n == 9
;     push qword 0        ; push a fake error code
; %endif
; %if n == 16
;     push qword 0        ; push a fake error code
; %endif
; %if n >= 18
;     push qword 0        ; push a fake error code
; %endif
; 
;     ;mov qword [Temp], rsp
;     push rax
;     mov rax, [rsp-8]
;     push rcx
;     push rdx
;     push rbx
;   .cia:                  ; FIXME: vykdant komandas su qword [Temp] 
;                          ; linker failina
;     ;push qword [Temp]
;     ;push rax
;     
;     push rbp
;     push rsi
;     push rdi
;     
;     push r8
;     push r9
;     push r10
;     push r11
;     push r12
;     push r13
;     push r14
;     push r15
;     
;     push qword n
;     mov rdi, rsp
;     
;     mov rax, qword n
;     cmp rax, 40
;     jb .ACK_MASTER_PIC
;     mov dx, 0xA0
;     mov ax, 0x20
;     out dx, al
;  .ACK_MASTER_PIC:
;     mov dx, 0x20
;     mov ax, 0x20
;     out dx, al
;                         ; FIXME: Meginant pasiekti interrupt_handlers
;                         ; adresa taip pat failina linker
; 
; ;     mov rax, default_interrupt_handler
;     call default_interrupt_handler
; 
;     add rsp, 8      ; Pop(qword n)
; 
;     pop r15
;     pop r14
;     pop r13
;     pop r12
;     pop r11
;     pop r10
;     pop r9
;     pop r8
;     
;     pop rdi
;     pop rsi
;     pop rbp
;    ; add rsp, 8      ; Pop(Temp)
;     pop rbx
;     pop rdx
;     pop rcx
;     pop rax
;     add rsp, 8      ; Pop(Error Code)

    iretq
%assign n n + 1
%endrep