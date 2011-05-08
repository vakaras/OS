========
Pastabos
========

Atmintis
========

Reali atmintis
--------------

Šios reikšmės yra gautos bandymu keliu, todėl vystant OS gali keistis:

+ 0x0000000000103000 – šiuo **fiziniu** adresu yra saugoma rodyklė į 
  puslapiavimo mechanizmą;
+ 0x00000000001fa000 – šiuo fiziniu adresu prasideda branduolio kodas?
  Tada jis turėtų atitikti virtualų adresą 0xffff800000000068.

Virtuali atmintis
-----------------

+ **[0xB8000; 0xB8FA0)** – atmintis rezervuota vaizdui.

+ [00000000 00000000; 00007FFF FFFFFFFF] – naudotojo atmintis;
+ [FFFF8000 00000000; FFFFFFFF FFFFFFFF] – OS atmintis;

Puslapiavimas
-------------

64 bitų operacinė sistema naudoja ``IA-32E`` puslapiavimo rėžimą 
(žr. *Intel 64 and IA32 Achitectures Software Developer's Manual*
*Volume 3 (3A & 3B): System Programming Guide* 142 puslapį). Esmė
ta, kad virtualus adresas konvertuojamas į fizinį pasinaudojant 
keturiomis 4 KB dydžio lentelėmis, vis siaurinant adreso sritį, kol
gaunamas puslapio adresas. Į aukščiausio lygio lentelę (``PML4``) rodo 
registras ``CR3``. 

+-----------+------------------------+-----------------+---------+
| Trumpinys | Lentelės pavadinimas   | Dokumentacijoje | Sritis  |
+===========+========================+=================+=========+
| pml       |                        | PML4            | 39-47   |
+-----------+------------------------+-----------------+---------+
| pdp       | page directory pointer | Directory Ptr   | 30-38   |
+-----------+------------------------+-----------------+---------+
| pd        | page directory         | Directory       | 21-29   |
+-----------+------------------------+-----------------+---------+
| pt        | page table             | Table           | 12-20   |      
+-----------+------------------------+-----------------+---------+

``bootloader`` paruošia tokią struktūrą:

+ pirmas gigabaitas virtualios atminties (pml[0], pdp[0], pd[*]) rodo į 
  pirmą gigabaitą fizinės atminties (kadangi naudojami 2MB puslapiai, 
  tai struktūra pt nenaudojama);

+ antroji pusė (adresai nuo 0xFFFF800000000000, 1 MB iš viso) yra 
  skirta pačiai OS (pml[256], pdp[0], pd[0], pt[*]).

  **Svarbu**: virtualus adresas 0xffff800000000000 atitinka 
  realų adresą 0x00000000001fa000, kitaip tariant branduolys prasideda
  nuo antrojo megabaito.

Pastaba: Norint naudoti lentelių reikšmes, kaip adresus, reikia *numesti*
paskutinius 12 bitų. Pavyzdžiui:

.. code-block:: cpp

  u64int *pml = (u64int *) 0x0000000000103000;  // VIM riktas*
  u64int *pdp1 = (u64int *) (pml[0] & PTINV);   // VIM riktas*
  u64int *pd1 = (u64int *) (pdp1[0] & PTINV);   // VIM riktas*

GDT
---

Paliekam tokį **GDT**, kokį mums paruošė ``bootloader``. Iš ``bochs``
(``data/bochsout.txt``) gauname, kad mūsų kodo segmentas yra 0x8, o
duomenų segmentas yra 0x10:

::

  03349000000i[CPU0 ] | SEG selector     base    limit G D
  03349000000i[CPU0 ] | SEG sltr(index|ti|rpl)     base    limit G D
  03349000000i[CPU0 ] |  CS:0008( 0001| 0|  0) 00000000 00000fff 1 0
  03349000000i[CPU0 ] |  DS:0010( 0002| 0|  0) 00000000 ffffffff 1 1
  03349000000i[CPU0 ] |  SS:0010( 0002| 0|  0) 00000000 ffffffff 1 1
  03349000000i[CPU0 ] |  ES:0010( 0002| 0|  0) 00000000 ffffffff 1 1
  03349000000i[CPU0 ] |  FS:0018( 0003| 0|  0) 00000000 ffffffff 1 1
  03349000000i[CPU0 ] |  GS:0018( 0003| 0|  0) 00000000 ffffffff 1 1

Pastabos
--------

+ Puslapiavimų aprašymai yra pateikti 118 puslapyje. 64 bitų architektūroje
  yra naudojamas ``IA-32E`` puslapiavimas (``CR0.PG = 1, CR4.PAE = 1, 
  IA32_EFER.LME = 1``).
+ ``IA-32E`` puslapiavimas yra aprašytas 142 puslapyje.
+ 125 puslapyje duota lentelė, kurioje nurodyta į kokią struktūrą kada yra
  kreipiamasi konvertuojant virtualų adresą į realų.

Multiprogramiškumas
===================

Ką reikia padaryti:

1.  išsikelti dėklą į kokią nors žinomą vietą prieš pereinant į C++ 
    kodą (tam, kad būtų žinoma tiksli jo vieta);
2.  ``cr3`` rodo į PML4, norint pakeisti puslapiavimo lentelę pakanka
    jį perrašyti?
3.  sukurti ``TSS`` (aprašymas 319 psl.)


Pastabos:

+   prieš persijungiant į kitą užduotį turi būti ``CR0.TS = 1`` 
    (aprašymas 77 psl., pavyzdys ``arcticos/Modules/Task/round_robin.c`` 
    170 eil.);

TODO
====

+ Realizuoti puslapiavimo mechanizmą.
  `Aprašymas <http://wiki.osdev.org/Page_Frame_Allocation>`_
  `Veikimo principas <http://wiki.osdev.org/Paging>`_
+ Realizuoti operatorius „new“ ir „delete“.
  `Aprašymas 2 <http://wiki.osdev.org/C%2B%2B#The_Operators_.27new.27_and_.27delete.27>`_
  `Kitas aprašymas <http://wiki.osdev.org/Memory_Allocation>`_

+ Išsiaiškinti, koks elementas, kokioje realios atminties vietoje gulės.
+ Išsiaiškinti, kokių tipų resursai bus sistemoje.
+ Išsiaiškinti, kokie procesai bus sistemoje.
+ Išsiaiškinti, už kokias funkcijas bus atsakingas OS branduolys.
+ Išsiaiškinti, kaip galima paleilsti naudotojo programą.
+ Įdiegti 
  `C Library <http://wiki.osdev.org/GCC_Cross-Compiler#Step_2_-_C_Library>`_
+ Įdiegti `libsupc++ <http://wiki.osdev.org/Libsupcxx>`_.

C funkcijų kvietimas
====================

Pirmieji šeši skaitiniai argumentai (nepriklausomai nuo tipo, gali būti
tiek ``char``, tiek ``long``) perduodami pradedant kairiausiu tokia
tvarka: ``RDI``, ``RSI``, ``RDX``, ``RCX``, ``R8`` ir ``R9``. Kiti 
argumentai yra perduodami per dėklą.

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

