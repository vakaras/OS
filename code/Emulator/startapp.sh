#!/bin/bash

if [ $# != 1 ] ; then
  echo "Usage: startapp.sh application-name"
elif [ -e apps/$1 ] ; then
  echo "App '$1' already exists."
else
  modulename=$(echo $1 | sed 's/-/_/g')
  cp --preserve=mode -r apps/.skeleton apps/$1 && \
  mkdir apps/$1/$modulename && \
  echo -e '#!/usr/bin/python3' >> apps/$1/$modulename/__init__.py && \
  sed -i \
      -e 's/$(name)/'$1'/g' \
      -e 's/$(author)/'$USER'/g' \
      -e 's/$(date)/'$(date +%Y-%m-%d)'/g' \
          apps/$1/CHANGES.txt \
          apps/$1/docs/conf.py \
          apps/$1/docs/index.rst \
          apps/$1/setup.py 
fi
