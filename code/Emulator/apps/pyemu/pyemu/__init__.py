#!/usr/bin/python
# -*- coding: utf-8 -*-

import sys

from pyemu.realmachine import RealMachine


def main(argv=None):
    """
    """

    if not argv:
        argv = sys.argv[:]

    if len(argv) != 2:
        print u'Naudojimas: pyemu <failas>'
        sys.exit(1)
    else:
        file = argv[1]

    rm = RealMachine()
    rm.load_virtual_machine(file)
    rm.processor.execute()

