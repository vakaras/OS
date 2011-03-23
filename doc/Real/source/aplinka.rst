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

  sudo apt-get install build-essential nasm libxpm-dev

+ **build-essential** yra meta paketas su visais įrankiais reikalingais 
  *C/C++* programų kompiliavimui.
+ **nasm** yra asembleris.
+ **libxpm-dev** yra biblioteka reikalinga emuliatoriaus kompiliavimui.

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

  # Prisijungiam prie serverio (kai paprašo slaptažodžio reikia tiesiog
  # paspausti „Įvesti“ klavišą):
  cvs -d:pserver:anonymous@bochs.cvs.sourceforge.net:/cvsroot/bochs login
  # Parsisiunčiame naujausią versiją (gali užtrukti):
  cvs -z3 -d:pserver:anonymous@bochs.cvs.sourceforge.net:/cvsroot/bochs checkout bochs
  cd bochs
  # „Atsukame“ versiją į stabilią (gali užtrukti):
  cvs update -d -r REL_2_4_1_FINAL

Dabar susikuriame naują failą ``.conf.mano``:
  
.. code-block:: bash

  #!/bin/bash

  ./configure \
      --prefix "${BOCHSDIR}" \
      --with-x11 \
      --enable-smp \
      --enable-x86-64 \
      --enable-plugins \
      --enable-disasm \
      --enable-show-ips 

+ ``"${BOCHSDIR}"`` – katalogas, kuriame bus išsaugoti sukompiliuotos
  programos failai.
+ ``--with-x11`` – nurodymas naudoti **X11** grafinę aplinką.
+ ``--enable-smp`` – nurodymas įkompiliuoti kelių procesorių palaikymą.
+ ``--enable-x86-64`` – nurodymas įkompiliuoti 64 bitų palaikimą.
+ ``--enable-show-ips`` – nurodymas rodyti atliktų instrukcijų kiekį.

Pakeičiame jo leidimus:

.. code-block:: bash
  
  chmod 755 .conf.mano

Kompiliuojame:

.. code-block:: bash

  ./.conf.mano && make && make install
