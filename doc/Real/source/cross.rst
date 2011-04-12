Kompiliatoriaus diegimas
========================

Pagal `http://wiki.osdev.org/GCC_Cross-Compiler`_

``${DEST}`` – kur išsaugoti sukompiluotą kompiliatorių.

.. code-block:: bash

  sudo apt-get install libmpc-dev

  export PREFIX="${DEST}"
  export TARGET=x86_64-elf

  wget -c http://ftp.gnu.org/gnu/gcc/gcc-4.6.0/gcc-core-4.6.0.tar.gz
  wget -c http://ftp.gnu.org/gnu/gcc/gcc-4.6.0/gcc-g++-4.6.0.tar.gz
  wget -c http://ftp.gnu.org/gnu/binutils/binutils-2.21.tar.gz

  tar -xvf binutils-2.21.tar.gz 
  tar -xvf gcc-core-4.6.0.tar.gz 
  tar -xvf gcc-g++-4.6.0.tar.gz 

``build-binutils`` ir ``build-gcc`` – katalogai, kuriuose vykdome 
kompiliavimą.

.. code-block:: bash

  mkdir build-binutils build-gcc

  # binutils
  cd build-binutils
  ../binutils-2.21/configure --target=$TARGET --prefix=$PREFIX --disable-nls
  make all
  make install

  # gcc
  cd ../build-gcc
  export PATH=$PATH:$PREFIX/bin
  ../gcc-4.6.0/configure --target=$TARGET --prefix=$PREFIX --disable-nls \
      --enable-languages=c,c++ --without-headers
  make all-gcc
  make install-gcc

  # libgcc
  make all-target-libgcc
  make install-target-libgcc

Padarome, kad galima būtų pasiekti sukompiliuotą kompiliatorių:

.. code-block:: bash
  
  ln -s "${DEST}/bin/* bin/
