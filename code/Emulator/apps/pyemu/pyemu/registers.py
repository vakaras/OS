#!/usr/bin/python
# -*- coding: utf-8 -*-


u""" Modulis, kuriame realizuotos klasės registrų ir atminties ląstelių
emuliacijai.
"""


WORD_SIZE = 8                           # Žodžio dydis yra 8 baitai.


def to_unicode(string):
    u""" Jei argumentas nėra unikodo eilutė, tai tada bandoma į ją
    konvertuoti. Daroma prielaida, kad koduotė yra **utf-8**.
    """
    if isinstance(string, unicode):
        return string
    else:
        return string.decode('utf-8')


def to_bytes(string):
    u""" Jei argumentas yra unikodo eilutė, tai tada bandoma ją konvertuoti
    į baitų eilutę. Naudojama koduotė yra **utf-8**.
    """
    if isinstance(string, unicode):
        return string.encode('utf-8')
    else:
        return str(string)


class Cell:
    u""" Atminties ląstelė. Turi dydį išreikštą simboliais.
    """

    def __init__(self, size=WORD_SIZE):
        u""" ``size`` – ląstelės dydis baitais.
        """

        self.size = size
        self._value = '0'*self.size

    def _set_value(self, value):
        u""" Patikrina ar reikšmė telpa atminties ląstelėje ir jei taip,
        tai ją priskiria. Jei ne tai išmeta ``ValueError``.
        """

        value = to_bytes(value)
        if len(value) <= self.size:
            self._value = value
        else:
            raise ValueError(u'Reikšmė netelpa ląstelėje.')

    def _get_value(self):
        u""" Grąžina ląstelėje saugomą reikšmę.
        """

        return self._value

    value = property(_get_value, _set_value)
