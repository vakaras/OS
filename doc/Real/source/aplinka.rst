=======
Aplinka
=======

Šiame skyriuje aprašoma, kaip paruošti savo kompiuterį darbui. Šio teksto 
autoriai dirbdami naudojo 64 bitų 
`Ubuntu Maverick Meerkat <http://www.ubuntu.lt/>`_ 
operacinę sistemą, taigi jei jūs naudojate, kokią nors kitą OS arba 
32 bitų OS versiją, tai kai kurie veiksmai gali skirtis.

Failų katalogo struktūra
========================

Naudojama failų sistemos struktūra yra tokia:

.. code-block:: bash
    
  OS/       # Šakninis katalogas su visais failais.
    src/    # Katalogas, kuriame laikomi visi išeities tekstai.
    bin/    # Katalogas, kuriame saugomos pagalbinės programos.


Kompiliavimas
=============

Įrankius reikalingus OS sukompiliavimui **Ubuntu** sistemoje įdiegti galima
terminale suvedus šią komandą:

.. code-block:: bash

  sudo apt-get install build-essential nasm 

+ **build-essential** yra meta paketas su visais įrankiais reikalingais 
  *C/C++* programų kompiliavimui.
+ **nasm** yra asembleris.

Emuliavimas
===========

Kadangi dirbant yra reikalinga nuolat patikrinti ar tai ką sukūrėme iš
tiesų veikia, tai tam naudosime `Bochs <http://bochs.sourceforge.net/>`_
emuliatorių. Kadangi mums reikia 64 bitų palaikimo, tai jį teks 
susikompiliuoti rankomis.

Tam pirmiausia mums reikia parsisiųsti *bochs* išeities tekstus 
(kaip tai padaryti išsamiai aprašyta 
`čia <http://bochs.sourceforge.net/doc/docbook/user/get-src-cvs.html>`_):

.. code-block:: bash

  # Prisijungiam prie serverio:
  cvs -d:pserver:anonymous@bochs.cvs.sourceforge.net:/cvsroot/bochs login
  # Parsisiunčiame naujausią versiją:
  cvs -z3 -d:pserver:anonymous@bochs.cvs.sourceforge.net:/cvsroot/bochs checkout bochs
  cd bochs
  # „Atsukame“ versiją į stabilią:
  cvs update -d -r REL_2_4_1_FINAL
