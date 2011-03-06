#!/usr/bin/python
# -*- coding: utf-8 -*-


u""" Testai.
"""


import unittest

from pyemu.realmachine import RealMachine
from pyemu.memory import RealMemory
from pyemu.processor import Processor

class RealMachineTest(unittest.TestCase):
    u""" Testai pagalbinėms funkcijoms.
    """

    def test_init_memory(self):

        r_mem = RealMemory()
        assert r_mem[356] == '00000000'
        r_mem[12, 11] = u'ačiū'
        assert r_mem[203] == '  a\xc4\x8di\xc5\xab'
                                        # 12 * 16 + 11 == 203

    def test_init_processor(self):

        r_mem = RealMemory()
        processor = Processor(r_mem)

    def test_init_real_machine(self):

        rm = RealMachine()


