#!/usr/bin/python
# -*- coding: utf-8 -*-


u""" Modulis, kuriame realizuotos klasės registrų ir atminties ląstelių
emuliacijai.
"""


from pyemu import exceptions

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


def int_to_hex(number, size=None):
    u""" Konvertuoja sveikąjį teigiamą skaičių ``number`` į simbolių eilutę
    išreiškiančią šešioliktainį sveiką teigiamą skaičių.

    Jei ``size`` yra ne None, tai pasirūpina, kad simbolių eilutė būtų
    ne trumpesnė nei ``size``.
    """

    string = hex(abs(number))[2:]
    if not size:
        return string
    else:
        return '{{0:0>{0}}}'.format(size).format(string)


def hex_to_int(number):
    u""" Konvertuoja simbolių eilutę išreiškiančią šešioliktainį sveiką
    teigiamą skaičių į ``int``.
    """

    return int('0x' + number, 16)


class Wrapper(object):
    u""" Klasė, kuri yra požymis ar tam tikra klasė yra apgaubiančioji.
    """

def set_descriptor(x, name, desc):
    u""" Funkcija, kuri leidžia objektui priskirti deskriptorių.

    (Normaliai deskriptorius gali turėti tik klasės.)
    """
    t = type(x)
    if not issubclass(t, Wrapper):
        class AWrap(Wrapper, t): 
            pass
        x.__class__ = AWrap
    setattr(x.__class__, name, desc)


class Cell(object):
    u""" Atminties ląstelė. Turi dydį išreikštą simboliais.
    """

    def __init__(self, size=WORD_SIZE, handler=None):
        u"""

        + ``size`` – ląstelės dydis baitais.
        + ``handler`` – funkcija, kuri yra iškviečiama, kai pasikeičia
          ląstelės reikšmė.
        """

        self.size = size
        self._format = '{{0:>{0}}}'.format(self.size)
        self._value = '0'*self.size
        if not handler:
            self.handler = lambda : None
        else:
            self.handler = handler

    def set_value(self, value):
        u""" Patikrina ar reikšmė telpa atminties ląstelėje ir jei taip,
        tai ją priskiria. Jei ne tai išmeta ``ValueError``.
        """
        # TODO: Pridėti pranešimo išsiuntimą, jog reikšmė pasikeitė.
        # Tam, kad būtų paprasčiau parašyti grafinę sąsają.

        value = to_bytes(value)
        if len(value) <= self.size:
            self._value = self._format.format(value)
            self.handler()
        else:
            raise exceptions.ValueError(u'Reikšmė netelpa ląstelėje.')
        return self

    def get_value(self):
        u""" Grąžina ląstelėje saugomą reikšmę.
        """

        return self._value

    def __str__(self):
        u""" Grąžina ląstelėje saugomą reikšmę, kaip ``str`` tipo objektą.
        """

        return str(self._value)

    value = property(get_value, set_value)


class Register(Cell):
    u""" Bendro pobūdžio registras.
    """

    def __get__(self, obj, objtype):
        """ Getter'is išorinei klasei. (Žr. „Python descriptor“)
        """

        return self._value

    def __set__(self, obj, value):
        """ Setter'is išorinei klasei. (Žr. „Python descriptor“)
        """

        self.value = value
        return self


class IntegerRegister(Register):
    u""" Registras skirtas sveikiesiems skaičiams su ženklu saugoti.
    """

    def __init__(self, size=WORD_SIZE):
        u""" ``size`` – registro dydis baitais.
        """

        super(IntegerRegister, self).__init__(size)
        self._format = '{{0:>+{0}}}'.format(self.size)
        self.value = 0

    def set_value(self, value):
        u""" Patikrina ar reikšmė yra sveikas skaičius ir ar ji telpa. Tada
        ją priskiria. Klaidos atveju išmeta ``ValueError``.
        """

        try:
            value_as_integer = int(value)
        except ValueError:
            raise exceptions.ValueError(u'Reikšmė turi būti skaičius')
        value_as_string = self._format.format(value_as_integer)
        if len(value_as_string) <= self.size:
            self._value = value_as_string
        else:
            raise exceptions.ValueError(u'Reikšmė netelpa ląstelėje.')
        return self

    def __int__(self):
        u""" Gražina registro reikšmę, kaip sveikąjį skaičių.
        """

        return int(self._value)

    def __add__(self, number):
        u""" Gautą skaičių ``number`` sudeda su registro reikšme ir grąžina
        rezultatą.
        """

        return int(self) + int(number)

    def __sub__(self, number):
        u""" Gautą skaičių ``number`` atima iš registro reikšmės ir grąžina
        rezultatą.
        """

        return int(self) - int(number)

    def __get__(self, obj, objtype):
        """ Getter'is išorinei klasei. (Žr. „Python descriptor“)
        """

        return int(self)

    value = property(Register.get_value, set_value)


class HexRegister(Register):
    u""" Registras skirtas sveikiems neneigiamiems šešioliktainiams
    skaičiams saugoti.
    """

    def __init__(self, size=WORD_SIZE):
        u""" ``size`` – registro dydis baitais.
        """

        super(HexRegister, self).__init__(size + 2)
                                        # Kompensuojamas „0x“ dydis.
        self.size = size
        self.value = 0

    def set_value(self, value):
        u""" Patikrina ar reikšmė yra sveikas teigiamas skaičius ir ar ji
        telpa. Tada ją priskiria. Klaidos atveju išmeta ``ValueError``.
        """

        try:
            value_as_integer = int(value)
        except ValueError:
            raise exceptions.ValueError(u'Reikšmė turi būti skaičius')
        if value_as_integer < 0:
            raise exceptions.ValueError(
                    u'Turi būti sveikas teigiamas skaičius.')
        value_as_string = self._format.format(hex(value_as_integer))
        if len(value_as_string) <= self.size + 2:
                                        # Kompensuojamas „0x“ dydis.
            self._value = value_as_string
        else:
            raise exceptions.ValueError(u'Reikšmė netelpa ląstelėje.')
        return self

    def __int__(self):
        u""" Grąžina registro reikšmę, kaip sveikąjį skaičių.
        """

        return int(self._value, 16)

    def __add__(self, number):
        u""" Gautą skaičių ``number`` sudeda su registro reikšme ir grąžina
        rezultatą.
        """

        return int(self) + int(number)

    def __sub__(self, number):
        u""" Gautą skaičių ``number`` atima iš registro reikšmės ir grąžina
        rezultatą.
        """

        return int(self) - int(number)

    def __get__(self, obj, objtype):
        """ Getter'is išorinei klasei. (Žr. „Python descriptor“)
        """

        return int(self)

    value = property(Register.get_value, set_value)


class ChoiceRegister(Register):
    u""" Registras skirtas loginėms reikšmėms saugoti.
    """

    def __init__(self, choices):
        u""" ``choices`` – galimos registre įgyti reikšmės. Registro dydis
        baitais yra lygus ilgiausio galimo pasirinkimo ilgiui.
        """

        self.choices = [to_bytes(choice) for choice in choices]
        size = len(max(self.choices, key=len))
        super(ChoiceRegister, self).__init__(size)
        self.formated_choices = [
                self._format.format(choice) for choice in self.choices]
        self.value = self.choices[0]

    def set_value(self, value):
        u""" Prieš priskiriant patikrina ar norima priskirti reikšmė yra
        vienas iš pasirinkimų.
        """

        value_as_string = self._format.format(to_bytes(value))
        if value_as_string in self.formated_choices:
            self._value = value_as_string
        else:
            raise ValueError(u'Nežinomas pasirinkimas.')
        return self

    value = property(Register.get_value, set_value)


class StatusFlagRegister(object):
    u""" Dviejų baitų loginis registras.

    Šis registras nėra standartinis, jam negalima priskirti reiškmės.

    """

    def __init__(self):
        u""" Inicializuoja registrą.
        """

        self.__dict__['_bytes'] = {
                'CF': False,
                'ZF': False,
                'SF': False,
                'OF': False,
                }

    def __getattr__(self, flag):
        u""" Gražina nurodyto požymio (``flag``) reikšmę.
        """

        try:
            return (self._bytes[flag] and 1) or 0
        except KeyError:
            raise AttributeError(u'Nežinomas požymis.'.encode('utf-8'))

    def __setattr__(self, flag, value):
        u""" Nurodytam požymiui (``flag``) priskiria ``value`` reikšmę
        konvertuotą į loginę.
        """

        if self._bytes.has_key(flag):
            self._bytes[flag] = True if value else False
        else:
            raise AttributeError(u'Nežinomas požymis.'.encode('utf-8'))

    def toString(self):
        u""" Grąžina SF string reikšmę:
        
        CxZxSxOx, kur x = požymio reikšmė.
        """
        
        return 'C{0.CF} Z{0.ZF} S{0.SF} O{0.OF}'.format(self)
