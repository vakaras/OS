#!/usr/bin/python
# -*- coding: utf-8 -*-


u""" Modulis, kuriame realizuotos klasės mašinos atminties emuliacijos
realizacijai.
"""

from pyemu.registers import Cell


BLOCKS = 256
BLOCK_SIZE = 16
PAGER_SIZE = 16


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

    def _get_cell(self, address):
        u""" Grąžina atminties ląstelę, kuri yra nurodyta adresu.
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
        return self._cells[block][cell]

    def __getitem__(self, address):
        u""" Grąžina adresu nurodytos ląstelės reikšmę.
        """

        return self._get_cell(address).value

    def __setitem__(self, address, value):
        u""" Priskiria adresu nurodytai ląstelei nurodytą reikšmę.
        """

        self._get_cell(address).value = value


class VirtualMemory(object):
    u""" Virtualios mašinos atmintis. (Abstrakti klasė.)
    """

    PLR = None                          # TODO: Nuoroda į procesoriaus 
                                        # registrą.
    PLBR = None                         # TODO: Nuoroda į procesoriaus 
                                        # registrą.
    RAM = None                          # TODO: Nuoroda į realios mašinos 
                                        # atmintį.
