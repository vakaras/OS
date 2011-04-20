/* 
 * This file content was copied from JohnnyTheDon post at 
 * http://f.osdev.org/viewtopic.php?f=1&t=17548
 */
OUTPUT_FORMAT(elf32-i386)
ENTRY (loader)
SECTIONS{
   . = 0x00100000;

   .text ALIGN(4096) :{
      *(.text)
   }
   .rodata ALIGN (4096) : {
      *(.rodata)
   }

   .data ALIGN (4096) : {
      _start_code16 = .;
      *(.data)
      _end_code16 = .;
   }

   .bss ALIGN (4096) : {
      sbss = .;
      *(COMMON)
      *(.bss)
      ebss = .;
   }
}
