=========
Projektas
=========

Procesai
========

Sisteminiai procesai (veikia ``ring 0``):

+ ``init`` – menamas procesas, kuriame veikia operacinės sistemos 
  branduolys;
+ ``waitera`` ir ``waiterb`` – procesai, kurie užima procesorių 
  *laisvalaikiu*.

Taip pat sistemoje veikia naudotojo procesai, kurie veikia ``ring 3``.

Procesai ``waitera`` ir ``waiterb``
-----------------------------------

Šiuos abu procesus sukuria ``init`` operacinės sistemos startavimo metu.
Šie du procesai yra žemiausio prioriteto ir yra vykdomi tada, kai nėra
jokio kito pasiruošusio proceso.

``waitera`` veikimo algoritmas:

.. code-block:: python

  while True:
    create_resource(MessageWaiterA)
    get_resource(MessageWaiterB)

``waiterb`` veikimo algoritmas:

.. code-block:: python
  
  while True:
    get_resource(MessageWaiterA)
    create_resource(MessageWaiterB)

Naudotojo procesai
==================

Naudotojo procesai sistemoje atsiranda vienu iš dviejų būdų:

+ sukuria procesas ``init``, gavęs naudotojo nurodymą (per klaviatūrą);
+ naudotojo procesas sukuria dar vieną naudotojo procesą.

Naudotojo procesai vykdo programas, kurios yra įkompiliuotos į pačią
operacinę sistemą. Gavus nurodymą yra padaroma programos kopija, 
naujai išskirtoje atmintyje (kiekvienai programai yra skiriama 2 MB
operatyviosios atminties), procesorius yra perjungiamas į naudotojo
rėžimą ir yra „šokama“ į programos vykdomojo kodo pradžią. Programos
vykdymo metu atsiradusius pertraukimus apdoroja procesas ``init``.
Galimi pertraukimai:

+ ``int 0x80`` – kreipimasis į operacinės sistemos funkcijas, procesas
  pereina iš būsenos vykdomas į būseną pasiruošęs arba blokuotas;
+ ``int 0x20`` – laikrodžio pertraukimas, jam įvykus procesas pereina iš
  būsenos vykdomas į būseną pasiruošęs;
+ įvykus bet kuriam kitam pertraukimui, proceso vykdymas yra nutraukiamas.

Resursų sąrašas
===============

+ ``File`` kuria proceso ``init`` objektas ``FileManager``.
+ ``Screen`` (yra 4 tokio tipo resursai) kuria proceso ``init`` objektas
  ``Monitor``.
+ ``Page`` kuria proceso ``init`` objektas ``MemoryManager``.

.. figure:: resources.png
  :scale: 100%
  :alt: Resursų diagrama.

  Resursų diagrama.


Operacinės sistemos branduolys
==============================

Operacinė sistemos kodas yra vykdomas proceso ``init`` aplinkoje.

.. figure:: core.png
  :scale: 100%
  :alt: OS branduolio diagrama.

  OS branduolio diagrama.

Operacinės sistemos sąsaja
==========================

Naudotojo programa
------------------

Naudotojo programa OS funkcijas gali pasiekti per 0x80 pertraukimą. Kokią
funkciją reikia iškviesti nurodo registro ``rax`` reikšmė:

+   ``0`` – naudotojo programa baigė darbą;
+   ``1`` – atidaryti failą, kurio pavadinimas yra ``rbx`` reikšmė, 
    skaitymui (atidaryto failo id būna registre ``rax``);
+   ``2`` – atidaryti failą, kurio pavadinimas yra ``rbx`` reikšmė,
    rašymui (atidaryto failo id būna registre ``rbx``);
+   ``3`` – uždaryti failą, kurio id yra nurodytas registre ``rbx``;
+   ``4`` – ištrinti failą, kurio pavadinimas yra registro ``rbx`` reikšmė;
+   ``5`` – nuskaityti žodį (8 baitus) į ``rax`` iš atidaryto failo, kurio 
    id yra nurodytas registre ``rbx``;
+   ``6`` – įrašyti žodį (8 baitus) iš ``rdi`` į failą, kurio id yra 
    nurodytas registre ``rbx``;

Iškvietus OS funkciją su neteisingais parametrais arba sukėlus bet kurį kitą
pertraukimą, naudotojo programa turėtų būti nužudyta.

Servisas ``loader``
-------------------

Servisas ``loader`` be naudotojo programai pasiekiamų OS funkcijų dar gali 
prieiti prie tokių:

+   ``10`` – prašyti resurso ``LoadProgram`` (programos identifikatorius
    perduodamas, kaip registro ``rax`` reikšmė, resursas iš resursų sąrašo
    ištrinamas);
+   ``11`` – prašyti resurso ``Memory`` (resurso id perduodamas, kaip
    ``rbx`` reikšmė, o pml4 adresas perduodamas, kaip registro ``rdi``
    reikšmė);
+   ``12`` – prašyti resurso ``Screen`` (resurso id perduodamas, kaip
    ``rbx`` reikšmė);
+   ``13`` – sukurti naują procesą (``Memory`` resurso id perduodamas
    kaip ``rbx`` reikšmė, ``Screen`` resurso id perduodamas, kaip ``rdi``
    reikšmė, virtualus adresas į kurį reikia „šokti“ yra perduodamas
    registru ``rsi``, dėklo viršūnės virtualus adresas perduodamas 
    registru ``rdx``).


Servisai ``waitera`` ir ``waiterb``
-----------------------------------

Gal juos pakeisti vienu procesu ``waiter``, kuris su resursais nedirba? 
Bus lengviau realizuoti.

Išdėstymo atmintyje planas
==========================

Reali atmintis
--------------

Sistemos vaizdas realioje atmintyje.

+--------------------+-------+------------------------------------------+
| Adresas            | Dydis | Prasmė                                   |
+====================+=======+==========================================+
| 0x00000000001fa000 | 14 MB | Čia guli branduolio vykdomasis           |
+--------------------+       | kodas.                                   |
| 0x0000000000ff9fff |       |                                          |
|                    |       |                                          |
+--------------------+-------+------------------------------------------+
| 0x0000000000ffa000 |  2 MB | Branduolio dėklas. Pradinis              |
+--------------------+       | ``rsp =  0x10fa000``.                    |
| 0x00000000011f9fff |       |                                          |
|                    |       |                                          |
+--------------------+-------+------------------------------------------+
| 0x00000000011fa000 |  1 MB | Branduolio puslapiavimo lentelė.         |
+--------------------+       |                                          |
| 0x00000000012f9fff |       |                                          |
|                    |       |                                          |
+--------------------+-------+------------------------------------------+
| 0x00000000012fa000 |  1 MB | Serviso ``loader`` puslapiavimo          |
+--------------------+       | lentelė.                                 |
| 0x00000000013f9fff |       |                                          |
|                    |       |                                          |
+--------------------+-------+------------------------------------------+
| 0x00000000013fa000 |  1 MB | Serviso ``waitera`` (arba ``waiter``)    |
+--------------------+       | puslapiavimo lentelė.                    |
| 0x00000000014f9fff |       |                                          |
|                    |       |                                          |
+--------------------+-------+------------------------------------------+
| 0x00000000014fa000 |  1 MB | Serviso ``waiterb``                      |
+--------------------+       | puslapiavimo lentelė.                    |
| 0x00000000015f9fff |       |                                          |
|                    |       |                                          |
+--------------------+-------+------------------------------------------+
| 0x00000000015fa000 |  1 MB | 1 Naudotojo programos                    |
+--------------------+       | puslapiavimo lentelė.                    |
| 0x00000000016f9fff |       |                                          |
|                    |       |                                          |
+--------------------+-------+------------------------------------------+
| 0x00000000016fa000 |  1 MB | 2 Naudotojo programos                    |
+--------------------+       | puslapiavimo lentelė.                    |
| 0x00000000017f9fff |       |                                          |
|                    |       |                                          |
+--------------------+-------+------------------------------------------+
| 0x00000000017fa000 |  1 MB | 3 Naudotojo programos                    |
+--------------------+       | puslapiavimo lentelė.                    |
| 0x00000000018f9fff |       |                                          |
|                    |       |                                          |
+--------------------+-------+------------------------------------------+
| 0x00000000018fa000 |  1 MB | 4 Naudotojo programos                    |
+--------------------+       | puslapiavimo lentelė.                    |
| 0x00000000019f9fff |       |                                          |
|                    |       |                                          |
+--------------------+-------+------------------------------------------+
| 0x00000000019fa000 | 16 MB | 1 naudotojo programos atmintis.          |
+--------------------+       |                                          |
| 0x0000000001ff9fff |       |                                          |
|                    |       |                                          |
+--------------------+-------+------------------------------------------+
| 0x0000000001ffa000 | 16 MB | 2 naudotojo programos atmintis.          |
+--------------------+       |                                          |
| 0x0000000002ff9fff |       |                                          |
|                    |       |                                          |
+--------------------+-------+------------------------------------------+
| 0x0000000002ffa000 | 16 MB | 3 naudotojo programos atmintis.          |
+--------------------+       |                                          |
| 0x0000000003ff9fff |       |                                          |
|                    |       |                                          |
+--------------------+-------+------------------------------------------+
| 0x0000000003ffa000 | 16 MB | 4 naudotojo programos atmintis.          |
+--------------------+       |                                          |
| 0x0000000004ff9fff |       |                                          |
|                    |       |                                          |
+--------------------+-------+------------------------------------------+

Branduolio virtuali atmintis
----------------------------

TODO

Serviso ``loader`` virtuali atmintis
------------------------------------

Lygiai tokia pat, kaip ir branduolio.

Serviso ``waiter`` virtuali atmintis
------------------------------------

Lygiai tokia pat, kaip ir branduolio.

Naudotojo programos virtuali atmintis
-------------------------------------
