#!/usr/bin/python
# -*- coding: utf-8 -*-


u""" Testai.
"""


import os
import unittest

from pyemu.realmachine import RealMachine
from pyemu.memory import RealMemory, Pager
from pyemu.memory import VirtualMemoryCode, VirtualMemoryData
from pyemu.processor import Processor

class RealMachineTest(unittest.TestCase):
    u""" Testai pagalbinėms funkcijoms.
    """


    def test_init_processor(self):

        r_mem = RealMemory()
        processor = Processor(r_mem)

    def test_init_real_machine(self):

        rm = RealMachine()
        tests_dir = os.path.abspath(os.path.dirname(__file__))
        rm.load_virtual_machine(os.path.join(tests_dir, 'test_program_1'))
        assert rm.processor.IC == 0
        rm.processor.step()
        assert rm.processor.IC == 1

        try:
            rm.load_virtual_machine(
                    open(os.path.join(tests_dir, 'test_program_1')))
        except Exception, e:
            assert str(e) == 'Not implemented!'

class ProcessorTest(unittest.TestCase):
    u""" Testai procesoriaus funkcijoms.
    """

    a = []

    def setUp(self):
        u""" Užkrauna mašiną testavimui.
        """
        self.a.append('setUp')
        print 'bla: ', self.a

        self.rm = RealMachine()
        self.a.append('rm:' + str(self.rm))
        tests_dir = os.path.abspath(os.path.dirname(__file__))
        self.rm.load_virtual_machine(
                os.path.join(tests_dir, 'test_program_1'))
        self.proc = self.rm.processor
        self.code = self.rm.virtual_memory_code
        self.data = self.rm.virtual_memory_data

    def test_command_LR1(self):
        self.a.append('LR1')
        print 'bla: ', self.a
        assert self.proc.IC == 0
        print '->', self.proc.R1, '<-'
        assert self.proc.R1 == '00000000'
        assert self.proc.R2 == '00000000'
        self.code[0] = ' LR1 5'
        self.data[5] = 230
        self.proc.R1 = 5
        assert self.proc.R1 == '       5'
        assert self.proc.R2 == '00000000'
        assert self.proc.step()
        assert self.proc.R1 == '     230'
        assert self.proc.R2 == '00000000'
        assert self.proc.IC == 1

    def test_command_LR2(self):
        self.a.append('LR2')
        print 'bla: ', self.a
        assert self.proc.IC == 0
        assert self.proc.R1 == '00000000'
        assert self.proc.R2 == '00000000'
        self.code[0] = ' LR2 5'
        self.data[5] = 230
        self.proc.R2 = 5
        print self.proc.R1
        assert self.proc.R1 == '00000000'
        assert self.proc.R2 == '       5'
        assert self.proc.step()
        assert self.proc.R2 == '     230'
        assert self.proc.IC == 1

    def tearDown(self):
        u""" Ištrina mašiną.
        """

        self.a.append('tearDown')
        print 'bla: ', self.a

        del self.rm
        del self.proc
        del self.code
        del self.data


class RealMemoryTest(unittest.TestCase):
    u""" Testai atminties funkcijoms.
    """

    def test_init_memory(self):

        r_mem = RealMemory()

    def test_helper_functions(self):

        r_mem = RealMemory()

        assert r_mem.get_address_tuple((0, 0)) == (0, 0)
        assert r_mem.get_address_tuple(0) == (0, 0)
        assert r_mem.get_address_tuple(203) == (12, 11)
        assert r_mem.get_address_tuple((12, 11)) == (12, 11)

        assert r_mem.get_address_int((0, 0)) == 0
        assert r_mem.get_address_int(0) == 0
        assert r_mem.get_address_int(203) == 203
        assert r_mem.get_address_int((12, 11)) == 203

    def test_cell_access(self):

        r_mem = RealMemory()

        assert r_mem[356] == '00000000'
        r_mem[12, 11] = u'ačiū'
        assert r_mem[203] == '  a\xc4\x8di\xc5\xab'
                                        # 12 * 16 + 11 == 203

        word = '01234567'
        r_mem.put_data((0, 0), word * 16)
        r_mem.put_data((1, 0), word * 5 + 'aaa')
        r_mem.put_data((1, 15), word)
        try:
            r_mem.put_data((1, 16), word)
        except ValueError, e:
            assert unicode(e) == u'Duomenys netelpa į bloką.'
        else:
            self.fail(u'Turėjo būti išmesta išimtis.'.encode('utf-8'))

        assert r_mem.get_data((0, 0), 8 * 16) == word * 16
        assert r_mem.get_data((1, 0), 8 * 6) == word * 5 + 'aaa     '
        assert r_mem.get_data((1, 0), 8 * 5 + 3) == word * 5 + 'aaa'

        assert r_mem.get_byte((1, 2), 5) == '5'
        assert r_mem.get_byte((1, 2), 8 * 3 + 2) == 'a'
        r_mem.set_byte((1, 2), 8 * 3 + 2, 'b')
        assert r_mem[1, 5] == 'aab     '

    def test_pager(self):

        r_mem = RealMemory()

        try:
            pager = Pager(r_mem, C=0, D=1)
        except ValueError, e:
            assert unicode(e) == \
                    u'Kodo segmento dydis turi būti didesnis už 1.'
        else:
            self.fail(u'Turėjo būti išmesta išimtis.'.encode('utf-8'))
        try:
            pager = Pager(r_mem, C=1, D=-1)
        except ValueError, e:
            assert unicode(e) == \
                    u'Duomenų segmento dydis turi būti teigiamas.'
        else:
            self.fail(u'Turėjo būti išmesta išimtis.'.encode('utf-8'))

        pager = Pager(r_mem, C=1, D=1)

        assert r_mem.get_data((0, 0), 8 + 36) == (
                "01011011"
                "00000000"
                "00000000"
                "00000000"
                "00000000"
                "0000"
                )

        pager = Pager(r_mem, address=0)

        assert pager.get_byte(0) == '0'
        assert pager.get_byte(1) == '1'
        assert pager.get_code_cell_address((0, 4)) == (16, 4)
        assert pager.get_data_cell_address((0, 5)) == (17, 5)
        try:
            pager.get_code_cell_address((1, 4))
        except ValueError, e:
            assert unicode(e) == \
                    u'Virtualus adresas nepriklauso kodo segmentui.'
        else:
            self.fail(u'Turėjo būti išmesta išimtis.'.encode('utf-8'))
        try:
            pager.get_data_cell_address((1, 4))
        except ValueError, e:
            assert unicode(e) == \
                    u'Virtualus adresas nepriklauso duomenų segmentui.'
        else:
            self.fail(u'Turėjo būti išmesta išimtis.'.encode('utf-8'))

    def test_virtual_memory(self):

        r_mem = RealMemory()
        pager = Pager(r_mem, C=1, D=1)

        vmcode = VirtualMemoryCode(r_mem, pager)
        r_mem[16, 4] = '01234567'
        assert vmcode[0, 4] == '01234567'
        vmcode[0, 5] = 'abababab'
        assert r_mem[16, 5] == 'abababab'

        vmdata = VirtualMemoryData(r_mem, pager)
        r_mem[17, 4] = '01234567'
        assert vmdata[0, 4] == '01234567'
        vmdata[0, 5] = 'abababab'
        assert r_mem[17, 5] == 'abababab'

    def test_creating_virtual_memory(self):

        r_mem = RealMemory()

        code = [
                'namo:LR1 00a',
                'LR2 00a\n',
                'CMP    ',
                u'JE «namo»  \n'.encode('utf-8'),
                ]
        data = [
                '[a]:labas\n\n\n\n',
                ]
        vmcode, vmdata = r_mem.create_virtual_memory(code, 1, data, 1)
        assert vmcode[0] == 'LR1 00a '
        assert r_mem.get_data((16, 0), 8 * 16) == (
                'LR1 00a '
                'LR2 00a '
                'CMP     '
                'JE 000  ' + '00000000' * 12)
        assert vmdata[10] == 'labas   '
        assert r_mem.get_data((17, 0), 8 * 16) == (
                '00000000' * 10 + 'labas   ' + '00000000' * 5)
