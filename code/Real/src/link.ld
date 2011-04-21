OUTPUT_FORMAT(elf64-x86-64)
ENTRY(_start)
KERNEL_VMA = 0xFFFF800000000000;
SECTIONS
{
    . = KERNEL_VMA;

    .text : AT(ADDR(.text) - KERNEL_VMA)
    {
        _code = .;
        *(.text)
        *(.gnu.linkonce.t*)
        . = ALIGN(4096);
    }

   .data : AT(ADDR(.data) - KERNEL_VMA)
   {
        _data = .;
        *(.data)
        *(.gnu.linkonce.d*)
        . = ALIGN(4096);
   }

   .rodata : AT(ADDR(.rodata) - KERNEL_VMA)
   {
        start_ctors = .;                /* C++ globalūs ir statiniai */
                                        /* konstruktoriai. */
        *(.ctor*)
        end_ctors = .;

        start_dtors = .;                /* C++ globalūs ir statiniai */
                                        /* destruktoriai. */
        *(.dtor*)
        end_dtors = .;

        *(.rodata*)
        *(.gnu.linkonce.r*)

   }

   .ehframe : AT(ADDR(.ehframe) - KERNEL_VMA)
   {
       _ehframe = .;
       *(.ehframe)
        . = ALIGN(4096);
   }

   .bss : AT(ADDR(.bss) - KERNEL_VMA)
   {
       _bss = .; 
       *(.bss) 
       *(.COMMON*)
       *(.gnu.linkonce.b*)
       . = ALIGN(4096);
   }

   _end = .;

   /DISCARD/ :
   {
        *(.comment)
   }
}
