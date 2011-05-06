; This file content was copied from JohnnyTheDon post at 
; http://f.osdev.org/viewtopic.php?f=1&t=17548

global loader
global _dbgwrite
extern _start_code16
extern _end_code16
extern BuildPageTables
extern MoveMultiboot

%define ALIGN (01b) ; We need the modules aligned
%define MEMINFO (010b) ; We need memory info
%define VIDINFO (0000b) ; We need video info
%define FLAGS (ALIGN | MEMINFO | VIDINFO) ; Flags passed to multiboot
%define MAGIC (01BADB002h) ; Multiboot magic number
%define CHECKSUM -(MAGIC + FLAGS) ; Multiboot Checksum must be 0 when added to magic and flags

%define STACK_SIZE 0200000h
%define MULTIBOOT_BUFFER_SIZE 0F0000h
%define KERNEL_BUFFER_SIZE 0200000h

%define CODE16_RELOC 01000h
%define CODE16_PNP_BUFFER 02000h

%define PAGE_TABLE_MASK 11011b

%define NEWLINE 0Ah

%macro dbgwrite 1 ;writes a string to port E9, which is debug output in bochs
   push %1   
   call _dbgwrite
   add esp, 4
%endmacro

section .text
align 4
dd MAGIC
dd FLAGS
dd CHECKSUM
dq 0 ; reserve space for load information
dq 0
dd 0
dd 0 ; set linear graphics mode
dd 0 ; no preference for width
dd 0 ; no preference for height
dd 0 ; no preference for bits per pixel (BPP)
load_str db 'Double Load Succeeded.',NEWLINE, 0
no64_str db 'No 64-bit Support Detected! What the fuck?!',NEWLINE, 0
ret16_str db 'Successfully returned from 16-bit code.',NEWLINE, 0
multiboot_info_fail_str db 'Multiboot Info Loading Failed.',NEWLINE, 0
kernel_invalid_str db 'The Kernel File Was Invalid.',NEWLINE, 0

BITS 32

loader:
   mov esp, _end_stack ; setup a stack
   dbgwrite load_str
check_for_64:
   pusha ; save all registers before cpuid
   mov eax, 080000001h
   cpuid
   test edx, (1<<29) ; check if the processor supports long (64-bit) mode
   jnz move_multiboot_info
   dbgwrite no64_str
move_multiboot_info:
   popa ; restore all registers after cpuid
   push multiboot_buffer ; push address of multiboot_buffer
   push ebx ; push address of origional multiboot info
   call MoveMultiboot
   add esp, 8 ; adjust stack after call
   cmp eax, 0 ; check for success (return value = 0)
   je move_code16
   dbgwrite multiboot_info_fail_str
   hlt
move_code16: ; moves the 16 bit code below the 1mb mark
   mov esi, _start_code16 ; Start of 16-bit code in this file
   mov edi, CODE16_RELOC ; Address 16-bit code needs to be loaded at
   mov ecx, _end_code16
   sub ecx, _start_code16 ; calculate size of 16-bit code
   test ecx, 01b ; Checks if the size is divisible by 2
   jz move_code16_fast ; If it is, a faster version of the move can be used
   cld ; clear direction flag
   rep movsb ; move the code one byte at a time
   jmp enter_code16
move_code16_fast: ; faster version of the code16 moving code
   shr ecx, 1 ; divide size by 2
   test ecx, 01b ; Check if the size is still divisible by 2
   jz move_code16_even_faster ; If it is, an even faster version of the move can be used
   cld ; clear direction flag
   rep movsw ; move the code one word at a time
   jmp enter_code16
move_code16_even_faster:
   shr ecx, 1 ; divide the size by 2
   cld
   rep movsd ; move the code one dword at a time
enter_code16: ; enters the 16-bit code
   mov edx, ret_code16 ; tell 16bit code where to returnmov eax, cr0
   jmp CODE16_RELOC ; enter 16 bit code. NOTE: We are not in real mode yet. the 16-bit code will switch to real mode.
ret_code16:
   dbgwrite ret16_str
build_page_tables: ; build temporary page tables
   mov eax, 0 ; setting tables to 0   
   mov edi, pml4 ; we are clearing all page structures, starting with pml4
   mov ecx, (4096*6) ; clearing 6 4096 byte structures (6 pages)
   cld
   rep stosb ; clear the structures
   push pt1
   push pd2
   push pd1
   push pdp2
   push pdp1
   push pml4
   push kernel_buffer
   push entry_point
   push multiboot_buffer
   call BuildPageTables
   add esp, 9*4 ; Adjust stack after function call
   cmp eax, 0
   je enter_64
   dbgwrite kernel_invalid_str
   hlt
enter_64:
   mov eax, cr4
   or eax, 10100000b ; set physical address extension (PAE) and page size extension (PSE) flags
   mov cr4, eax
   mov ecx, 0C0000080h ; select EFER Model Specific Register
   rdmsr
   or eax, 0100h ; set long mode enable (LME) flag
   wrmsr
   mov eax, pml4
   mov cr3, eax ; set address of pml4 (cr3)
   mov eax, cr0
   or eax, 080000000h ; set paging (PG) flag, which automatically sets LMA
   mov cr0, eax
   lgdt [gdtr]
   jmp 08h:in_64
BITS 64
in_64:
   mov rsp, _end_stack ; setup stack
   mov rbp, rsp
enter_kernel:
   mov rdi, multiboot_buffer
   call enter_kernel_stub ; Call into the kernel, located at start of higher half
   hlt ; if the kernel returns, halt
   jmp $
enter_kernel_stub:
   mov rax, [entry_point]
   jmp rax ; Jump to kernel
BITS 32

_dbgwrite:
   push ebp
   mov ebp, esp
   push ebx
   push eax
   mov ebx, dword [ebp+8]
_dbgwrite_loop:
   mov al, byte [ebx]
   inc ebx
   cmp al, 0
   je _dbgwrite_ret
   ;out 0E9h, al
   jmp _dbgwrite_loop
_dbgwrite_ret:
   pop eax
   pop ebx
   pop ebp
   ret

gdtr:
dw (_end_gdt-gdt)-1
dd gdt
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


section .data

incbin "main16.bin" ; include 16-bit binary code

section .bss

entry_point resq 1;

align 4096

pml4:
resq 512
pdp1:
resq 512
pdp2:
resq 512
pd1:
resq 512
pd2:
resq 512
pt1:
resq 512
pt2:
resq 512


multiboot_buffer:
resb MULTIBOOT_BUFFER_SIZE
_end_multiboot_buffer:

align 4096

kernel_buffer:
resb KERNEL_BUFFER_SIZE
_end_kernel_buffer:

stack:
resb STACK_SIZE
_end_stack:
