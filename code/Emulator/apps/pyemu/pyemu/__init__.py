#!/usr/bin/python
# -*- coding: utf-8 -*-

import sys

from pyemu.realmachine import RealMachine
from pyemu.filesystem import file_system

import wx

def main(argv=None):
    """
    """

    if not argv:
        argv = sys.argv[:]

    gui = False
    if len(argv) == 2:
        file = argv[1]
    elif len(argv) == 3 and argv[1] == 'gui':
        gui = True
        file = argv[2]
    else:
        print u'Naudojimas: pyemu <failas>'
        sys.exit(1)

    rm = RealMachine()

    if gui:
        app = wx.App()
        frame = wx.Frame(None, -1, 'Bla')
        frame.Show()
        app.MainLoop()
    else:

        def stdin():
            return raw_input('INPUT: ')

        def stdout(block):
            for i in range(0, len(block), 8):
                print 'Word {0:2}: "{1}"'.format(i/8, block[i:i+8])

        rm.load_virtual_machine(file)#, stdin, stdout)
        for name in file_system.get_files():
            print 'Failas {0}'.format(name)
        rm.processor.execute()
