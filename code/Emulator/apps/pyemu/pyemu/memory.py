#!/usr/bin/python
# -*- coding: utf-8 -*-


u""" Modulis, kuriame realizuotos klasės mašinos atminties emuliacijos
realizacijai.
"""

import re

from math import ceil, floor

from pyemu.registers import WORD_SIZE
from pyemu.registers import int_to_hex, hex_to_int
from pyemu.registers import Cell
from pyemu.filesystem import file_system


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

    def __init__(self, memory, address=None, C=None, D=None,
            stdin_handler=None, stdout_handler=None):
        u""" Jei ``address`` nėra None, tai puslapiavimo mechanizmą nuskaito
        iš atminties. Kitu atveju jį sukuria pagal gautuosius ``C`` ir
        ``D``.

        + ``memory`` – realios mašinos atmintis.
        + ``address`` – pirmo baito, kuriuo prasideda puslapiavimo įrašas
          atmintyje adresas.
        + ``C`` – kodo segmento dydis blokais.
        + ``D`` – duomenų segmento dydis blokais.
        + ``stdin_handler`` – funkcija, kuri yra iškviečiama, kai norima
          nuskaityti bloką iš standartinės įvesties.
        + ``stdout_handler`` – funkcija, kuriai yra perduodama standartinė
          išvestis.

        """

        self.PLR = None
        self.PLBR = None
        self.memory = memory
        self.files = {}

        if not stdin_handler:
            self.stdin_handler = lambda : ' ' * WORD_SIZE * BLOCK_SIZE
        else:
            self.stdin_handler = stdin_handler

        if not stdout_handler:
            self.stdout_handler = lambda x: None
        else:
            self.stdout_handler = stdout_handler

        if address is not None:
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
            data.append(ih(i))
        for i in range(PAGER_SIZE + C, PAGER_SIZE + C + D):
            data.append(ih(i))
        data.append('i' + '0' * WORD_SIZE)
        data.append('o' + '0' * WORD_SIZE)
        data.append('0'*18)
        self.files = {
                0: self.create_reader(self.stdin_handler),
                1: self.create_writer(self.stdout_handler),}

        self.memory.put_data(
                self.PLR * BLOCK_SIZE + self.PLBR, ''.join(data))

    def get_file_descriptor(self, id):
        u""" Gražina nurodyto id failo deskriptorių.

        Forma (<rėžimas>, <failo pavadinimas>).
        """

        C = self.get_C()
        D = self.get_D()
        descriptor = []
        address = self.memory.get_address_int((self.PLR, self.PLBR))
        offset =  4 + 2 * (C + D) + id * 9
        for i in range(9):
            descriptor.append(self.memory.get_byte(address, offset + i))
        return (descriptor[0], ''.join(descriptor[1:]))

    def set_file_descriptor(self, id, mode, name):
        u""" Nustato nurodyto id failo deskriptorių.

        + ``id`` – failo id.
        + ``mode`` – failo atidarymo rėžimas.
        + ``name`` – failo pavadinimas.

        """

        if len(mode) != 1 or len(name) != 8:
            raise ValueError(u'Netinkamo ilgio argumentai')

        C = self.get_C()
        D = self.get_D()
        address = self.memory.get_address_int((self.PLR, self.PLBR))
        offset =  4 + 2 * (C + D) + id * 9
        descriptor = mode + name
        for i, byte in enumerate(descriptor):
            self.memory.set_byte(address, offset + i, byte)

    def file_open(self, name):
        u""" Atidaro failą skaitymui.

        Sukuria failo deskriptorių ir grąžina failo id.
        """

        for id in range(4):
            mode, tmp = self.get_file_descriptor(id)
            if mode == '0':
                self.files[id] = self.create_reader(
                        file_system.open(name).read)
                self.set_file_descriptor(id, 'r', name)
                return id
        raise Exception(u'Viršytas atidarytų failų limitas.')

    def file_create(self, name):
        u""" Atidaro failą rašymui.

        Sukuria failo deskriptorių ir grąžina fialo id.
        """

        for id in range(4):
            mode, tmp = self.get_file_descriptor(id)
            if mode == '0':
                self.files[id] = self.create_writer(
                        file_system.create(name).write)
                self.set_file_descriptor(id, 'w', name)
                return id
        raise Exception(u'Viršytas atidarytų failų limitas.')

    def file_close(self, id):
        u""" Uždaro nurodytą failą.
        """

        del self.files[id]
        self.set_file_descriptor(id, '0', '0' * WORD_SIZE)

    def file_delete(self, name):
        u""" Ištrina failą nurodytu pavadinimu.
        """

        file_system.delete(name)

    def file_read(self, id):
        u""" Nuskaito ir grąžina bloką iš failo.
        """

        return self.files[id]()

    def file_write(self, id, block):
        u""" Įrašo bloką į failą.
        """

        self.files[id](block)

    def create_reader(self, read):
        u""" Sukuria skaitymo funkciją, kuri užtikrina, kad gražintu bloką.
        """

        def reader():
            block = read()
            return block + ' ' * (BLOCK_SIZE * WORD_SIZE - len(block))

        return reader

    def create_writer(self, write):
        u""" Sukuria rašymo funkciją, kuri užtikrina, kad būtų įrašytas
        blokas.
        """

        def writer(block):
            write(block + ' ' * (BLOCK_SIZE * WORD_SIZE - len(block)))

        return writer

    def read(self, address):
        u""" Nuskaito iš atminties virtualios mašinos puslapiavimo lentelę.
        """
        self.PLR, self.PLBR = self.memory.get_address_tuple(address)
        for id in range(4):
            mode, name = self.get_file_descriptor(id)
            if mode == 'i':
                self.files[id] = self.create_reader(self.stdin_handler)
            elif mode == 'o':
                self.files[id] = self.create_writer(self.stdout_handler)
            elif mode == 'r':
                self.files[id] = self.create_reader(
                        file_system.open(name).read)
            elif mode == 'w':
                self.files[id] = self.create_writer(
                        file_system.create(name).write)

    def get_byte(self, offset):
        u""" Gražina puslapiavimo lentelės baitą, pasislinkusį nuo pradžios
        per ``offset``.
        """
        return self.memory.get_byte((self.PLR, self.PLBR), offset)

    def get_C(self):
        u""" Gražina kodo segmento dydį blokais.
        """
        return hex_to_int(self.get_byte(0) + self.get_byte(1))

    def get_D(self):
        u""" Gražina duomenų segmento dydį blokais.
        """
        return hex_to_int(self.get_byte(2) + self.get_byte(3))

    def get_code_cell_address(self, virtual_address):
        u""" Apskaičiuoja realų ląstelės adresą pagal kodo segmento
        virtualų adresą.
        """

        virtual_address = self.memory.get_address_int(virtual_address)
        C = self.get_C()

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
        C = self.get_C()
        D = self.get_D()

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

    def __init__(self, handler=None):
        u""" Inicializuoja tuščią atmintį.

        + ``handler`` – funkcija, kuri yra iškviečiama, kai yra paliečiame
          ląstelės reikšmė. Sintaksė: (<blokas>, <ląstelės adresas bloke>).
        """

        if not handler:
            self.handler = lambda x, y: None
        else:
            self.handler = handler
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
        self.handler(block, cell)
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
                (ceil(size / WORD_SIZE) -
                    floor(size / WORD_SIZE)) * WORD_SIZE)
        data = data + ' ' * fill
        words = [
                data[i:i+WORD_SIZE] for i in range(0, len(data), WORD_SIZE)]
        for i, word in enumerate(words):
            try:
                self[block, cell + i] = word
            except IndexError:
                raise ValueError(u'Duomenys netelpa į bloką.')

    def get_data(self, address, size):
        u""" Grąžina duomenis nuo nurodyto adreso.

        ``size`` – kiek baitų gražinti.
        """

        address = self.get_address_int(address)

        words = size / WORD_SIZE        # Kiek sveikų žodžių reikia 
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
        self[address] = word[0:offset] + value + word[offset+1:]

    def create_virtual_memory(
            self, code, code_size, data, data_size,
            stdin_handler=None, stdout_handler=None):
        u""" Išskiria virtualią atmintį ir į ją įkelia kodą bei duomenis.
        """

        pager = Pager(
                self, C=code_size, D=data_size,
                stdin_handler=stdin_handler, stdout_handler=stdout_handler)

        # Įkeliamas kodo segmentas.
        vmcode = VirtualMemoryCode(self, pager)
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
                        u'«{0}»'.format(label),
                        int_to_hex(labels[label], 3)).encode('utf-8')
            else:
                command = command.encode('utf-8')
            command += ' ' * (WORD_SIZE - len(command))
            vmcode[i] = command

        # Įkeliamas duomenų segmentas. 
        vmdata = VirtualMemoryData(self, pager)
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

    def get_block(self, virtual_block):
        u""" Grąžina nurodytą bloką.
        """

        block, cell = self.pager.get_data_cell_address((virtual_block, 0))
        return self.memory.get_data((block, 0), BLOCK_SIZE * WORD_SIZE)

    def set_block(self, virtual_block, data):
        u""" Priskiria nurodytą bloką.
        """

        block, cell = self.pager.get_data_cell_address((virtual_block, 0))
        self.memory.put_data((block, 0), data)
