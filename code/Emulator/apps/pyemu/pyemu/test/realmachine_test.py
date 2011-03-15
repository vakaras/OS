#!/usr/bin/python
# -*- coding: utf-8 -*-


u""" Testai.
"""


import os
import unittest

from pyemu import exceptions
from pyemu.exceptions import BadAddress, WrongOpCode, WrongFileDescriptor
from pyemu.exceptions import ReadingWriteOnlyFileError
from pyemu.exceptions import WritingReadOnlyFileError
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


    def setUp(self):
        u""" Užkrauna mašiną testavimui.
        """

        self.rm = RealMachine()
        tests_dir = os.path.abspath(os.path.dirname(__file__))
        self.rm.load_virtual_machine(
                os.path.join(tests_dir, 'test_program_1'))
        self.proc = self.rm.processor
        self.code = self.rm.virtual_memory_code
        self.data = self.rm.virtual_memory_data

    def test_environment(self):
        assert self.code.pager.get_C() == 1
        assert self.data.pager.get_D() == 1

    def test_command_NONE(self):
        self.code[0] = 'NONE'
        assert self.proc.PI == '0'
        assert self.proc.step() == False
        assert self.proc.PI == '1'

    def test_command_wrong(self):
        self.code[0] = 'HALT 5'
        assert self.proc.PI == '0'
        assert self.proc.step() == False
        assert self.proc.PI == '1'

    def test_command_LR1(self):
        assert self.proc.IC == 0
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
        self.code[1] = ' LR1 10'
        assert self.proc.PI == '0'
        assert self.proc.step() == False
        assert self.proc.PI == '1'

    def test_command_LR2(self):
        assert self.proc.IC == 0
        assert self.proc.R1 == '00000000'
        assert self.proc.R2 == '00000000'
        self.code[0] = ' LR2 5'
        self.data[5] = 230
        self.proc.R2 = 5
        assert self.proc.R1 == '00000000'
        assert self.proc.R2 == '       5'
        assert self.proc.step()
        assert self.proc.R2 == '     230'
        assert self.proc.IC == 1

    def test_command_SR1(self):
        self.code[0] = ' SR1 5'
        self.proc.R1 = 2
        assert self.proc.R1 == '       2'
        assert self.proc.step()
        assert self.data[5] == '       2'

    def test_command_SR2(self):
        self.code[0] = ' SR2 5'
        self.proc.R2 = 2
        assert self.proc.R2 == '       2'
        assert self.proc.step()
        assert self.data[5] == '       2'

    def test_command_ADD(self):
        self.code[0] = 'ADD'
        self.proc.R1 = 1
        assert self.proc.R1 == '       1'
        self.proc.R2 = 2
        assert self.proc.R2 == '       2'
        assert self.proc.step()
        assert self.proc.R1 == '       3'

    def test_command_ADD_1(self):
        self.code[0] = 'ADD'
        self.proc.R1 = -1
        assert self.proc.R1 == '      -1'
        self.proc.R2 = 2
        assert self.proc.R2 == '       2'
        assert self.proc.step()
        assert self.proc.R1 == '       1'

    def test_command_ADD_2(self):
        self.code[0] = 'ADD'
        self.proc.R1 = '99999999'
        assert self.proc.R1 == '99999999'
        self.proc.R2 = 2
        assert self.proc.R2 == '       2'
        try:
            assert self.proc._step()
        except exceptions.ValueError, e:
            assert unicode(e) == u'Reikšmė netelpa ląstelėje.'
        else:
            self.fail(u'Turėjo būti išmesta išimtis.'.encode('utf-8'))
        assert self.proc.R1 == '99999999'

    def test_command_ADDM(self):
        self.code[0] = 'ADDM 5'
        self.proc.R1 = 1
        assert self.proc.R1 == '       1'
        self.proc.R2 = 2
        assert self.proc.R2 == '       2'
        assert self.proc.step()
        assert self.data[5] == '       3'

    def test_command_SUB(self):
        self.code[0] = 'SUB'
        self.proc.R1 = 1
        assert self.proc.R1 == '       1'
        self.proc.R2 = 2
        assert self.proc.R2 == '       2'
        assert self.proc.step()
        assert self.proc.R1 == '      -1'

    def test_command_SUBM(self):
        self.code[0] = 'SUBM 5'
        self.proc.R1 = 1
        assert self.proc.R1 == '       1'
        self.proc.R2 = 2
        assert self.proc.R2 == '       2'
        assert self.proc.step()
        assert self.data[5] == '      -1'

    def test_command_DIV(self):
        self.code[0] = 'DIV'
        self.proc.R1 = 423423
        assert self.proc.R1 == '  423423'
        self.proc.R2 = 342
        assert self.proc.R2 == '     342'
        assert self.proc.step()
        assert self.proc.R1 == '    1238'
        assert self.proc.R2 == '      27'

    def test_command_MUL(self):
        self.code[0] = 'MUL'
        self.proc.R1 = 4233
        assert self.proc.R1 == '    4233'
        self.proc.R2 = 342
        assert self.proc.R2 == '     342'
        assert self.proc.step()
        assert self.proc.R1 == ' 1447686'

    def test_command_CMP(self):
        self.code[0] = 'CMP'
        self.proc.R1 = -1
        self.proc.R2 = 1
        assert self.proc.step()
        assert self.proc.SF.ZF == 0
        assert self.proc.SF.SF == 0

    def test_command_CMP_1(self):
        self.code[0] = 'CMP'
        self.proc.R1 = 1
        self.proc.R2 = 1
        assert self.proc.step()
        assert self.proc.SF.ZF == 1

    def test_command_CMP_2(self):
        self.code[0] = 'CMP'
        self.proc.R1 = 1
        self.proc.R2 = -1
        assert self.proc.step()
        assert self.proc.SF.ZF == 0
        assert self.proc.SF.SF == 1

    def test_command_JMP(self):
        self.code[0] = 'JMP 5'
        assert self.proc.step()
        assert self.proc.IC == 5

    def test_command_PD(self):

        assert self.proc.IC == 0
        self.code[0] = 'PD 1 0'
        assert self.proc.step() == True

        assert self.proc.IC == 1
        self.code[1] = 'PD a 0'
        try:
            self.proc._step()
        except WrongOpCode, e:
            assert unicode(e) == u'Netinkami komandos argumentai.'
        else:
            self.fail(u'Turėjo būti išmesta išimtis.'.encode('utf-8'))

        assert self.proc.IC == 2
        self.code[2] = 'PD 1 z'
        try:
            self.proc._step()
        except WrongOpCode, e:
            assert unicode(e) == u'Netinkami komandos argumentai.'
        else:
            self.fail(u'Turėjo būti išmesta išimtis.'.encode('utf-8'))

        assert self.proc.IC == 3
        self.code[3] = 'PD 5 0'
        try:
            self.proc._step()
        except WrongFileDescriptor, e:
            assert unicode(e) == u'Nežinomas id.'
        else:
            self.fail(u'Turėjo būti išmesta išimtis.'.encode('utf-8'))

        assert self.proc.IC == 4
        self.code[4] = 'PD 1 30'
        try:
            self.proc._step()
        except BadAddress, e:
            assert unicode(e) == \
                    u'Virtualus adresas nepriklauso duomenų segmentui.'
        else:
            self.fail(u'Turėjo būti išmesta išimtis.'.encode('utf-8'))

    def test_command_PDR(self):
        assert self.proc.IC == 0
        self.proc.R1 = 1
        self.code[0] = 'PDR 0'
        assert self.proc.step() == True

        assert self.proc.IC == 1
        self.proc.R1 = 'a'
        self.code[1] = 'PDR 0'
        try:
            self.proc._step()
        except exceptions.ValueError, e:
            assert unicode(e) == u'Reikšmė turi būti skaičius'
        else:
            self.fail(u'Turėjo būti išmesta išimtis.'.encode('utf-8'))

        assert self.proc.IC == 2
        self.proc.R1 = 1
        self.code[2] = 'PDR z'
        try:
            self.proc._step()
        except WrongOpCode, e:
            assert unicode(e) == u'Netinkami komandos argumentai.'
        else:
            self.fail(u'Turėjo būti išmesta išimtis.'.encode('utf-8'))

        assert self.proc.IC == 3
        self.proc.R1 = 5
        self.code[3] = 'PDR 0'
        try:
            self.proc._step()
        except WrongFileDescriptor, e:
            assert unicode(e) == u'Nežinomas id.'
        else:
            self.fail(u'Turėjo būti išmesta išimtis.'.encode('utf-8'))

        assert self.proc.IC == 4
        self.proc.R1 = 1
        self.code[4] = 'PDR 30'
        try:
            self.proc._step()
        except BadAddress, e:
            assert unicode(e) == \
                    u'Virtualus adresas nepriklauso duomenų segmentui.'
        else:
            self.fail(u'Turėjo būti išmesta išimtis.'.encode('utf-8'))

    def test_command_GD(self):

        assert self.proc.IC == 0
        self.code[0] = 'GD 0 0'
        assert self.proc.step() == True

        assert self.proc.IC == 1
        self.code[1] = 'GD a 0'
        try:
            self.proc._step()
        except WrongOpCode, e:
            assert unicode(e) == u'Netinkami komandos argumentai.'
        else:
            self.fail(u'Turėjo būti išmesta išimtis.'.encode('utf-8'))

        assert self.proc.IC == 2
        self.code[2] = 'GD 0 z'
        try:
            self.proc._step()
        except WrongOpCode, e:
            assert unicode(e) == u'Netinkami komandos argumentai.'
        else:
            self.fail(u'Turėjo būti išmesta išimtis.'.encode('utf-8'))

        assert self.proc.IC == 3
        self.code[3] = 'GD 5 0'
        try:
            self.proc._step()
        except WrongFileDescriptor, e:
            assert unicode(e) == u'Nežinomas id.'
        else:
            self.fail(u'Turėjo būti išmesta išimtis.'.encode('utf-8'))

        assert self.proc.IC == 4
        self.code[4] = 'GD 0 20'
        try:
            self.proc._step()
        except BadAddress, e:
            assert unicode(e) == \
                    u'Virtualus adresas nepriklauso duomenų segmentui.'
        else:
            self.fail(u'Turėjo būti išmesta išimtis.'.encode('utf-8'))

    def tearDown(self):
        u""" Ištrina mašiną.
        """

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

    def test_cell_change_handler(self):

        a = []
        def handler(block, cell):
            a.append((block, cell))
        r_mem = RealMemory(handler=handler)
        assert len(a) == 0
        r_mem[(18, 11)] = 2
        assert len(a) == 1
        assert a[0] == (18, 11)

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
                "i00000000"
                "o00000000"
                "000000000"
                "000000000"
                )

        pager = Pager(r_mem, address=0)

        assert pager.get_byte(0) == '0'
        assert pager.get_byte(1) == '1'
        assert pager.get_code_cell_address((0, 4)) == (16, 4)
        assert pager.get_data_cell_address((0, 5)) == (17, 5)
        try:
            pager.get_code_cell_address((1, 4))
        except BadAddress, e:
            assert unicode(e) == \
                    u'Virtualus adresas nepriklauso kodo segmentui.'
        else:
            self.fail(u'Turėjo būti išmesta išimtis.'.encode('utf-8'))
        try:
            pager.get_data_cell_address((1, 4))
        except BadAddress, e:
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
        vmdata.set_block(0, '01234567' * 16)
        assert vmdata.get_block(0) == '01234567' * 16

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

    def test_io(self):

        rm = RealMemory()

        def stdin():
            return '01234567' * 15

        out = []
        def stdout(block):
            out.append(block)

        pager = Pager(
                rm, C=1, D=1, stdin_handler=stdin, stdout_handler=stdout)

        assert pager.file_read(0) == '01234567' * 15 + ' ' * 8
        try:
            pager.file_write(0, 'Sveikas pasauli!')
        except WritingReadOnlyFileError, e:
            assert unicode(e) == \
                    u'Bandoma rašyti į failą atidarytą rašymui.'
        else:
            self.fail(u'Turėjo būti išmesta išimtis.'.encode('utf-8'))

        assert len(out) == 0
        pager.file_write(1, 'Sveikas pasauli!')
        assert len(out) == 1
        assert out[0] == 'Sveikas ' + 'pasauli!' + ' ' * 8 * 14
        try:
            pager.file_read(1)
        except ReadingWriteOnlyFileError, e:
            assert unicode(e) == \
                    u'Bandoma skaityti iš failo atidaryto rašymui.'
        else:
            self.fail(u'Turėjo būti išmesta išimtis.'.encode('utf-8'))

        assert rm.get_data((0, 0), 8 + 36) == (
                "01011011"
                "i00000000"
                "o00000000"
                "000000000"
                "000000000"
                )
        pager.file_close(0)
        assert rm.get_data((0, 0), 8 + 36) == (
                "01011011"
                "000000000"
                "o00000000"
                "000000000"
                "000000000"
                )
        try:
            pager.file_read(0)
        except WrongFileDescriptor, e:
            assert unicode(e) == u'Nežinomas id.'
        else:
            self.fail(u'Turėjo būti išmesta išimtis.'.encode('utf-8'))

        try:
            pager.file_create('namas')
        except ValueError, e:
            assert unicode(e) == u'Netinkamo ilgio argumentai'
        else:
            self.fail(u'Turėjo būti išmesta išimtis.'.encode('utf-8'))
        assert pager.file_create('tmpfile1') == 0
        assert rm.get_data((0, 0), 8 + 36) == (
                "01011011"
                "wtmpfile1"
                "o00000000"
                "000000000"
                "000000000"
                )
        try:
            pager.file_create('tmpfile1')
        except ValueError, e:
            assert unicode(e) == u'Failas tokiu vardu jau atidarytas.'
        else:
            self.fail(u'Turėjo būti išmesta išimtis.'.encode('utf-8'))
        try:
            pager.file_open('tmpfile1')
        except ValueError, e:
            assert unicode(e) == u'Failas tokiu vardu jau atidarytas.'
        else:
            self.fail(u'Turėjo būti išmesta išimtis.'.encode('utf-8'))

        assert pager.file_create('tmpfile2') == 2
        assert pager.file_create('tmpfile3') == 3
        assert rm.get_data((0, 0), 8 + 36) == (
                "01011011"
                "wtmpfile1"
                "o00000000"
                "wtmpfile2"
                "wtmpfile3"
                )
        try:
            pager.file_create('tmpfile4')
        except Exception, e:
            assert unicode(e) == u'Viršytas atidarytų failų limitas.'
        else:
            self.fail(u'Turėjo būti išmesta išimtis.'.encode('utf-8'))
        try:
            pager.file_open('tmpfile4')
        except Exception, e:
            assert unicode(e) == u'Viršytas atidarytų failų limitas.'
        else:
            self.fail(u'Turėjo būti išmesta išimtis.'.encode('utf-8'))
        pager.file_write(2, 'trkkrt')
        pager.file_close(2)
        assert pager.file_open('tmpfile2') == 2
        assert rm.get_data((0, 0), 8 + 36) == (
                "01011011"
                "wtmpfile1"
                "o00000000"
                "rtmpfile2"
                "wtmpfile3"
                )
        pager = Pager(rm, address=0)
        assert rm.get_data((0, 0), 8 + 36) == (
                "01011011"
                "wtmpfile1"
                "o00000000"
                "rtmpfile2"
                "wtmpfile3"
                )
        assert pager.file_read(2) == 'trkkrt  ' + ' ' * 8 * 15
        try:
            pager.file_delete('tmpfile3')
        except ValueError, e:
            assert unicode(e) == u'Failas tokiu vardu atidarytas.'
        else:
            self.fail(u'Turėjo būti išmesta išimtis.'.encode('utf-8'))
        pager.file_close(0)
        pager.file_close(1)
        pager.file_close(2)
        pager.file_close(3)
        pager.file_delete('tmpfile1')
        pager.file_delete('tmpfile2')
        pager.file_delete('tmpfile3')
