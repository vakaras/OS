#!/usr/bin/python
# -*- coding: utf-8 -*-

import sys


def main(argv=None):
    """
    """

    if not argv:
        argv = sys.argv[:]

    for arg in argv:
        print arg
