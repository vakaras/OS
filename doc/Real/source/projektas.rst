=========
Projektas
=========

Atmintis
========


+ **[0xB8000; 0xB8FA0)** – atmintis rezervuota vaizdui.

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
