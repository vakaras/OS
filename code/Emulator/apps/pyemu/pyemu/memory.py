#!/usr/bin/python
# -*- coding: utf-8 -*-


u""" Modulis, kuriame realizuotos klasės mašinos atminties emuliacijos
realizacijai.
"""

import re

from pyemu.registers import WORD_SIZE
from pyemu.registers import int_to_hex, hex_to_int
from pyemu.registers import Cell


BLOCKS = 256
BLOCK_SIZE = 16
PAGER_SIZE = 16


def ih(number):
    u""" Gautąjį ``number`` konvertuoja į šešioliktainį skaičių
    dviejuose baituose.
    """
    string = int_to_hex(number, 2)
    assert len(string) == 2
    return string


class Pager(object):
    u""" Pagalbinis objektas tvarkymuisi su puslapiavimo mechanizmu.
    """

    def __init__(self, memory, address=None, C=None, D=None):
        u""" Jei ``address`` nėra None, tai puslapiavimo mechanizmą nuskaito
        iš atminties. Kitu atveju jį sukuria pagal gautuosius ``C`` ir
        ``D``.

        + ``memory`` – realios mašinos atmintis.
        + ``address`` – pirmo baito, kuriuo prasideda puslapiavimo įrašas
          atmintyje adresas.
        + ``C`` – kodo segmento dydis blokais.
        + ``D`` – duomenų segmento dydis blokais.

        """

        self.PLR = None
        self.PLBR = None
        self.memory = memory

        if address:
            self.read(address)
        else:
            self.create(C, D)

    def create(self, C, D):
        u""" Sukuria virtualios mašinos puslapiavimo lentelę. Nustato
        PLR ir PLBR.
        """

        self.PLR = 0
        self.PLBR = 0

        data = []
        if C < 1:
            raise ValueError(
                    u'Kodo segmento dydis turi būti didesnis už 1.')
        else:
            data.append(ih(C))
        if D < 0:
            raise ValueError(
                    u'Duomenų segmento dydis turi būti teigiamas.')
        else:
            data.append(ih(D))
        for i in range(PAGER_SIZE, PAGER_SIZE + C):
            code.append(ih(i))
        for i in range(PAGER_SIZE + C, PAGER_SIZE + C + D):
            data.append(ih(i))
        data.append('0'*36)

        self.memory.put_data(
                self.PLR * BLOCK_SIZE + self.PLR, ''.join(data))

    def read(self, address):
        u""" Nuskaito iš atminties virtualios mašinos puslapiavimo lentelę.
        """
        self.PLR, self.PLBR = self.memory.get_address_tuple(address)

    def get_byte(self, offset):
        u""" Gražina puslapiavimo lentelės baitą, pasislinkusį nuo pradžios
        per ``offset``.
        """
        return self.memory.get_byte((self.PLR, self.PLBR), offset)

    def get_code_cell_address(self, virtual_address):
        u""" Apskaičiuoja realų ląstelės adresą pagal kodo segmento
        virtualų adresą.
        """

        virtual_address = self.memory.get_address_int(virtual_address)
        C = hex_to_int(self.get_byte(0) + self.get_byte(1))

        min_address = 0
        max_address = C * BLOCK_SIZE
        if not (min_address <= virtual_address <= max_address):
            raise ValueError(
                    u'Virtualus adresas nepriklauso kodo segmentui.')
        virtual_block, cell = self.memory.get_address_tuple(virtual_address)

        block = hex_to_int(
                self.get_byte(4 + 2 * virtual_block) +
                self.get_byte(4 + 2 * virtual_block + 1))
        return block, cell

    def get_data_cell_address(self, virtual_address):
        u""" Apskaičiuoja realų ląstelės adresą pagal duomenų segmento
        virtualų adresą.
        """

        virtual_address = self.memory.get_address_int(virtual_address)
        C = hex_to_int(self.get_byte(0) + self.get_byte(1))
        D = hex_to_int(self.get_byte(2) + self.get_byte(3))

        min_address = 0
        max_address = D * BLOCK_SIZE
        if not (min_address <= virtual_address <= max_address):
            raise ValueError(
                    u'Virtualus adresas nepriklauso duomenų segmentui.')
        virtual_block, cell = self.memory.get_address_tuple(virtual_address)

        block = hex_to_int(
                self.get_byte(4 + 2 * (C + virtual_block)) +
                self.get_byte(4 + 2 * (C + virtual_block) + 1))
        return block, cell



class RealMemory(object):
    u""" Realios mašinos atmintis.
    """

    def __init__(self):
        u""" Inicializuoja tuščią atmintį.
        """

        self._cells = []
        for i in range(BLOCKS):
            block = []
            for j in range(BLOCK_SIZE):
                block.append(Cell())
            self._cells.append(block)


    def get_address_tuple(self, address):
        u""" Grąžina bloko ir elemento bloke adresus.

        Jei ``address`` yra ``tuple`` tipo objektas, tai laikoma, kad
        pirmas elementas ir bloko adresas, o ląstelės bloke. Jei
        ``address`` yra ``int`` tipo objektas, tai bloko adresas
        apskaičiuojamas ``address / BLOCK_SIZE``, o ląstelės bloke
        ``address % BLOCK_SIZE``.
        """

        if isinstance(address, int):
            block = address / BLOCK_SIZE
            cell = address % BLOCK_SIZE
        else:
            block, cell = address
        return block, cell

    def get_address_int(self, address):
        u""" Grąžina globalų adresą.

        Ši funkcija yra atvirkštinė funkcijai ``get_address_tuple``
        """

        if not isinstance(address, int):
            block, cell = address
            address = block * BLOCK_SIZE + cell
        return address

    def _get_cell(self, address):
        u""" Grąžina atminties ląstelę, kuri yra nurodyta adresu.
        """

        block, cell = self.get_address_tuple(address)
        return self._cells[block][cell]

    def __getitem__(self, address):
        u""" Grąžina adresu nurodytos ląstelės reikšmę.
        """

        return self._get_cell(address).value

    def __setitem__(self, address, value):
        u""" Priskiria adresu nurodytai ląstelei nurodytą reikšmę.
        """

        self._get_cell(address).value = value

    def put_data(self, address, data):
        u""" Nurodytu adresu į atmintį pakrauna duomenis ``data``.

        + Jei duomenys netelpa į tą patį bloką, išmeta išimti
          ``ValueError``.
        + Jei duomenys nesidalina lygiai į žodžius, tai lygiuoja
          į kairę ir trūkstamą dalį užpildo tarpais.

        """

        block, cell = self.get_address_tuple(address)

        # Apvalina iki žodžių.
        size = float(len(data))
        fill = int(
                (ceil(size / BLOCK_SIZE) -
                    floor(size / BLOCK_SIZE)) * BLOCK_SIZE)
        data = data + ' ' * fill
        words = [
                data[i:i+WORD_SIZE] for i in range(0, len(data), WORD_SIZE)]
        for i, word in enumerate(words):
            self[block, cell + i] = word
                                        # TODO: Patikrinti kada meta 
                                        # KeyError

    def get_data(self, address, size):
        u""" Grąžina duomenis nuo nurodyto adreso.

        ``size`` – kiek baitų gražinti.
        """

        address = self.get_address_int(address)

        words = size % WORD_SIZE        # Kiek sveikų žodžių reikia 
                                        # grąžinti.
        data = []
        for i in range(address, address + words):
            data.append(self[i])
        if words * WORD_SIZE < size:
            # Jei reikia gražinti dar žodžio dalį.
            word = self[address + words]
            data.append(word[:size - words * WORD_SIZE])
        return ''.join(data)

    def get_byte(self, address, offset):
        u""" Grąžina baitą, kuris yra nuo žodžio nurodyto ``address``
        paslinktas per ``offset`` baitų.
        """

        address = self.get_address_int(address) + offset / WORD_SIZE
        offset %= WORD_SIZE

        return self[address][offset]

    def set_byte(self, address, offset, value):
        u""" Pakeičia baito, kuris yra nuo žodžio nurodyto ``address``
        paslinktas per ``offset`` baitų, reikšmę.
        """

        address = self.get_address_int(address) + offset / WORD_SIZE
        offset %= WORD_SIZE

        word = self[address]
        return word[0:offset] + value + word[offset+1:]

    def create_virtual_memory(self, code, code_size, data, data_size):
        u""" Išskiria virtualią atmintį ir į ją įkelia kodą bei duomenis.
        """

        pager = Pager(self, C=code_size, D=data_size)

        # Įkeliamas kodo segmentas.
        vmcode = VirtualMemoryCode(memory, pager)
        labels = {}
        clean_code = []
        for i, line in enumerate(code):
            if ':' in line:
                label, command = line.split(':')
                labels[label.strip()] = i
            else:
                command = line
            clean_code.append(command.strip().decode('utf-8'))

        for i, command in enumerate(clean_code):
            if u'«' in command:
                label = command.split(u'«', 1)[1].split(u'»', 1)[0]
                command = command.replace(
                        u'«{{0}}»'.format(label),
                        labels[label]).encode('utf-8')
            else:
                command.encode('utf-8')
            command += ' ' * (WORD_SIZE - len(command))
            vmcode[i] = command

        # Įkeliamas duomenų segmentas. 
        vmdata = VirtualMemoryData(memory, pager)
        clean_data = {}
        address = 0
        for i, line in enumerate(data):
            result = re.search('^\[(?P<address>\w+)\]:', line)
            if result:
                hex_address = result.groupdict()['address']
                line = line[len('[{0}]:'.format(hex_address)):]
                address = hex_to_int(hex_address)
                if not clean_data.has_key(address):
                    clean_data[address] = []
            line = line.replace('\n', '')
            clean_data[address].append(line)

        word_format = '{{0:<{0}}}'.format(WORD_SIZE)
        for address, lines in clean_data.items():
            data = ''.join(lines)
            for i, j in enumerate(range(0, len(data), WORD_SIZE)):
                vmdata[address + i] = word_format.format(
                        data[j:j + WORD_SIZE])
        return vmcode, vmdata


class VirtualMemoryCode(object):
    u""" Virtualios mašinos atmintis, kodo segmentas.
    """

    def __init__(self, memory, pager):
        u"""
        + ``memory`` – realios mašinos atmintis.
        + ``pager`` – puslapiavimo mechanizmo objektas.
        """

        self.memory = memory
        self.pager = pager

    def __getitem__(self, address):
        u""" Grąžina adresu nurodytos kodo segmento ląstelės adresą.
        """

        return self.memory[self.pager.get_code_cell_address(address)]

    def __setitem__(self, address, value):
        u""" Priskiria adresu nurodytai ląstelei nurodytą reikšmę.
        """

        self.memory[self.pager.get_code_cell_address(address)] = value


class VirtualMemoryData(object):
    u""" Virtualios mašinos atmintis, duomenų segmentas.
    """

    def __init__(self, memory, pager):
        u"""
        + ``memory`` – realios mašinos atmintis.
        + ``pager`` – puslapiavimo mechanizmo objektas.
        """

        self.memory = memory
        self.pager = pager

    def __getitem__(self, address):
        u""" Grąžina adresu nurodytos kodo segmento ląstelės adresą.
        """

        return self.memory[self.pager.get_data_cell_address(address)]

    def __setitem__(self, address, value):
        u""" Priskiria adresu nurodytai ląstelei nurodytą reikšmę.
        """

        self.memory[self.pager.get_data_cell_address(address)] = value
