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
    data/   # Katalogas, kuriame saugomi įvairūs failai.


Kompiliavimas
=============

Įrankius reikalingus OS sukompiliavimui **Ubuntu** sistemoje įdiegti galima
terminale suvedus šią komandą:

.. code-block:: bash

  sudo apt-get install build-essential nasm libxpm-dev xorriso

+ **build-essential** yra meta paketas su visais įrankiais reikalingais 
  *C/C++* programų kompiliavimui.
+ **nasm** yra asembleris.
+ **libxpm-dev** yra biblioteka reikalinga emuliatoriaus kompiliavimui.
+ **xorriso** reikalingas grub2 paleidimo atvaizdžiui sukurti.

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

Sukuriame simbolinę nuorodą ``OS/bin`` kataloge:

.. code-block:: bash

  ln -s ${BOCHSDIR}/bin/* bin/

Sukuriame nustatymų failą ``OS/data/bochsrc.txt``:

.. code-block:: bash

  megs: 32
  romimage: file=${BOCHSDIR}/share/bochs/BIOS-bochs-latest
  vgaromimage: file=${BOCHSDIR}/share/bochs/VGABIOS-elpin-2.40
  ata0-master: type=cdrom, path="data/sgrub.iso", status=inserted
  boot: cdrom
  log: data/bochsout.txt
  mouse: enabled=0
  clock: sync=realtime
  cpu: ips=500000

Kaip ir anksčiau ``${BOCHSDIR}`` reikia pakeisti katalogo, į kurį buvo
įdiegtas ``bochs`` adresu.

Naudingi scenarijai
===================

+ Kompiliavimo automatizavimas failas ``OS/src/Makefile``.

  Pastaba: norint 64 bitų kompiuteryje sukompiliuoti 32 bitų OS reikia
  į ``LDFLAGS`` pridėti ``-melf_i386`` ir į ``CPPFLAGS`` pridėti
  ``-m32``.

  Pastaba: norint sukompiliuoti 64 bitų kompiuteryje 64 bitų OS reikia
  nustatyti ``ASFLAGS=-felf64``.

+ Saistymo failas ``OS/src/link.ld``.
+ Diskelio atvaizdo atnaujinimas ``bin/update_image``.
+ Emuliatoriaus paleidimas ``bin/run_bochs``.

OS paleidimas iš floppy
=======================

Pridedam punktą į GRUB meniu. Sukurti failą ``/etc/grub.d/50_custom``:

.. code-block:: bash

  #!/bin/sh
  set -e

  IMAGES=/boot/images
  . /usr/lib/grub/grub-mkconfig_lib
  if test -e /boot/memdisk ; then
    MEMDISKPATH=$( make_system_path_relative_to_its_root "/boot/memdisk" )
    echo "Found memdisk: $MEMDISKPATH" >&2
    find $IMAGES -name "* .img" | sort | # VIM riktas 
    while read image ; do
        IMAGEPATH=$( make_system_path_relative_to_its_root "$image" )
        echo "Found floppy image: $IMAGEPATH" >&2
        cat << EOF
  menuentry "Bootable floppy: $(basename $IMAGEPATH | sed s/.img//)" {
  EOF
        prepare_grub_to_access_device ${GRUB_DEVICE_BOOT} | sed -e "s/^/\t/"
        cat << EOF
          linux16 $MEMDISKPATH bigraw
          initrd16 $IMAGEPATH
  }
  EOF
    done
  fi

Ir atnaujinti GRUB su komanda:

.. code-block:: bash

  update-grub

OS paleidimas iš HDD
====================

Sukompiliuotą kernel failą keliame į **/boot/** katalogą.

Pridedam punktą į GRUB meniu **/boot/grub/grub.cfg**:

.. code-block:: bash

  menuentry 'l-HDD-k-OS (Laptop HDD Killer OS)' {
	  multiboot /boot/kernel
	  boot
  }
  
Paleisti kompiuterį iš naujo GRUB meniu pasirenkant naująją opciją

GRUB 2 naudojimas su bochs
==========================

Parsisiunčiame `Super Grub2 Disk 
<http://www.supergrubdisk.org/category/download/supergrub2diskdownload/>`_
išeities tekstus
`iš <http://prdownload.berlios.de/supergrub/supergrub-1.98s1.tar.gz>`_ 
ir išarchivuojame:

.. code-block:: bash

  mv supergrub-1.98s1.tar.gz data/
  cd data
  tar -xvf supergrub-1.98s1.tar.gz
  ln -s supergrub-1.98s1 supergrub-src

Pataisome numatytąjį ``grub.cfg``:

.. code-block:: bash

  echo "menuentry 'l-HDD-k-OS (Laptop HDD Killer OS)' {" \
    >> supergrub-src/menus/grub.cfg
  echo "  multiboot /boot/grub/kernel boot" >> supergrub-src/menus/grub.cfg
  echo "}" >> supergrub-src/menus/grub.cfg

TODO: Sutavarkyti (64-bit)
==========================

Pasiruošimas, kai įrankiai sukompiliuoti ir ką tik padarytas ``git clone``:

.. code-block:: bash

  bin/init \
    ~/Studijos/Programos/bochs \
    ~/Studijos/Programos/cross/bin \
    ~/Atsiuntimai/supergrub-1.98s1.tar.gz 
  cd src
  make
  cd bootloader
  make
  cd ../..
  bin/update_cd
  bin/run_bochs

Emuliavimas su QEMU + gdb
=========================

`Idėja <http://weichong78.blogspot.com/2010/04/vmware-qemu-and-gdb-for-boot-time-real.html>`_

Paleidžiam ``QEMU``:

.. code-block:: bash

  qemu -s -S -cpu coreduo -cdrom data/sgrub.iso

Prisijungiam su ``gdb``:

.. code-block:: bashprompt

  $ gdb
  # ...
  (gdb) target remote localhost:1234
