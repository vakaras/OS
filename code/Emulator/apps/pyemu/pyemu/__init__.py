#!/usr/bin/python
# -*- coding: utf-8 -*-

import sys

from pyemu.realmachine import RealMachine

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
        rm.load_virtual_machine(file)
        rm.processor.execute()
