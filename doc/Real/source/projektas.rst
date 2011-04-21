=========
Projektas
=========

Atmintis
========


+ **[0xB8000; 0xB8FA0)** – atmintis rezervuota vaizdui.

Virtuali atmintis:

+ [00000000 00000000; 00007FFF FFFFFFFF] – naudotojo atmintis;
+ [FFFF8000 00000000; FFFFFFFF FFFFFFFF] – OS atmintis;

Perdaryti:

+ 32 bitų sistemoje puslapiavimo lentelė yra 4 MB dydžio, jos adresas
  privalo būti lygiuotas 4 KB.

TODO
====

+ Realizuoti puslapiavimo mechanizmą.
  `Aprašymas <http://wiki.osdev.org/Page_Frame_Allocation>`_
  `Veikimo principas <http://wiki.osdev.org/Paging>`_
+ Realizuoti operatorius „new“ ir „delete“.
  `Aprašymas <http://wiki.osdev.org/C%2B%2B#The_Operators_.27new.27_and_.27delete.27>`_
  `Kitas aprašymas <http://wiki.osdev.org/Memory_Allocation>`_

+ Išsiaiškinti, koks elementas, kokioje realios atminties vietoje gulės.
+ Išsiaiškinti, kokių tipų resursai bus sistemoje.
+ Išsiaiškinti, kokie procesai bus sistemoje.
+ Išsiaiškinti, už kokias funkcijas bus atsakingas OS branduolys.
+ Išsiaiškinti, kaip galima paleilsti naudotojo programą.
+ Įdiegti 
  `C Library <http://wiki.osdev.org/GCC_Cross-Compiler#Step_2_-_C_Library>`_
+ Įdiegti `libsupc++ <http://wiki.osdev.org/Libsupcxx>`_.

Pastabos
========

C funkcijų kvietimas
--------------------

Pirmieji šeši skaitiniai argumentai (nepriklausomai nuo tipo, gali būti
tiek ``char``, tiek ``long``) perduodami pradedant kairiausiu tokia
tvarka: ``RDI``, ``RSI``, ``RDX``, ``RCX``, ``R8`` ir ``R9``. Kiti 
argumentai yra perduodami per steką.

Šie registrai, taip pat ir ``RAX``, ``R10`` ir ``R11`` yra pakeičiami
kviečiant funkciją.

`Šaltinis. <http://www.nasm.us/doc/nasmdo11.html>`_

NASM funkcijos kvietimas iš C kodo.

.. code-block:: nasm
  
  [GLOBAL foo]
  foo:
    mov rax, rdi

.. code-block:: cpp

  extern "C" int foo(int);
  // ...
  int a = foo(5);                       ; a įgyja reikšmę 5.

C funkcijos kvietimas iš NASM.

.. code-block:: cpp
  
  extern "C" int bar(int a, int b) {
    return a + b;
    }

.. code-block:: nasm

  [EXTERN bar]
  mov rdi, 0x1
  mov rsi, 0x2
  call bar                        
  jmp $                                 ; rax reikšmė yra 0x3

