#!/usr/bin/python
# -*- coding: utf-8 -*-


u""" Testai.
"""


import unittest
from pyemu.registers import to_unicode, to_bytes


class Utils(unittest.TestCase):
    u""" Testai pagalbinėms funkcijoms.
    """

    def test_unicode_str(self):

        s = 'tekstas'
        u = u'lietuviškas'

        assert to_unicode(s) == u'tekstas'
        assert to_unicode(u) == u'lietuviškas'
        assert to_bytes(s) == 'tekstas'
        assert to_unicode(to_bytes(u)) == u'lietuviškas'





