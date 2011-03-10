#!/usr/bin/python
# -*- coding: utf-8 -*-


import inspect

from pyemu.registers import Register, IntegerRegister, HexRegister
from pyemu.registers import ChoiceRegister, StatusFlagRegister
from pyemu.registers import hex_to_int


class Commands(object):
    u""" Objektas representuojantis komandų sistemą.
    """

    def __init__(self):
        u""" Susiindeksuoja visas savo komandas.
        """

        self.commands = dict([
                (name, function) \
                for name, function in \
                inspect.getmembers(self, inspect.isfunction) \
                ])

    def __getitem__(self, command):
        u""" Gražina komandų sistemos komandą.
        """

        return self.commands[command]

    @staticmethod
    def LR1(proc, x):
        proc.R1 = proc.virtual_memory_data[hex_to_int(x)]

    @staticmethod
    def LR2(proc, x):
        proc.R2 = proc.virtual_memory_data[hex_to_int(x)]

    @staticmethod
    def SR1(proc, x):
        proc.virtual_memory_data[hex_to_int(x)] = self.R1

    @staticmethod
    def SR2(proc, x):
        proc.virtual_memory_data[hex_to_int(x)] = proc.R2

    @staticmethod
    def ADD(proc):
        r1 = IntegerRegister(proc.R1.size).value = proc.R1
        r2 = IntegerRegister(proc.R2.size).value = proc.R2
        proc.R1 = int(r1) + int(r2)
        # FIXME: Kaip turi suformuoti SF požymius?

    @staticmethod
    def ADDM(proc, x):
        r1 = IntegerRegister(proc.R1.size).value = proc.R1
        r2 = IntegerRegister(proc.R2.size).value = proc.R2
        proc.virtual_memory_data[hex_to_int(x)] = int(r1) + int(r2)
        # FIXME: Kaip turi suformuoti SF požymius?

    @staticmethod
    def SUB(proc):
        r1 = IntegerRegister(proc.R1.size).value = proc.R1
        r2 = IntegerRegister(proc.R2.size).value = proc.R2
        proc.R1 = int(r1) - int(r2)
        # FIXME: Kaip turi suformuoti SF požymius?

    @staticmethod
    def SUBM(proc, x):
        r1 = IntegerRegister(proc.R1.size).value = proc.R1
        r2 = IntegerRegister(proc.R2.size).value = proc.R2
        proc.virtual_memory_data[hex_to_int(x)] = int(r1) - int(r2)
        # FIXME: Kaip turi suformuoti SF požymius?

    #@staticmethod
    #def <++>(proc<++>):
        #<++>

    @staticmethod
    def CMP(proc):
        if proc.R1 > proc.R2:
            proc.SF.ZF = 0
            proc.SF.SF = 0
        elif proc.R1 == proc.R2:
            proc.SF.ZF = 1
        else:
            proc.SF.ZF = 0
            proc.SF.SF = 1

    @staticmethod
    def JE(proc, x):
        if proc.SF.ZF == 1:
            proc.IC = x


class Processor(object):
    u""" Realios mašinos procesorius.
    """

    IC = HexRegister(3)                 # Nurodo vykdomos komandos adresą
                                        # atmintyje.
    R1 = Register()                     # Žodžio ilgio bendro naudojimo 
                                        # registras.
    R2 = Register()                     # Žodžio ilgio bendro naudojimo 
                                        # registras.
    PLR = HexRegister(2)                # Puslapių lentelės bloko adresas.
    PLBR = HexRegister(2)               # Puslapių lentelės pirmojo baito
                                        # adresas bloke.
    MODE = ChoiceRegister(['N', 'S'])   # Nurodo procesoriaus darbo rėžimą:
                                        # „N“ – naudotojo, 
                                        # „S“ – supervizoriaus.
    SF = StatusFlagRegister()           # Aritmetinių operacijų loginės
                                        # reikšmės.

    PI = ChoiceRegister([0, 1])         # Programiniai pertraukimai.
    SI = ChoiceRegister([0, 1])         # Supervizoriniai pertraukimai.
    TI = ChoiceRegister([0, 1])         # Laikrodžio pertraukimai.
    IOI = HexRegister(1)                # Įvedimo / išvedimo pertraukimai.
    CHST = HexRegister(1)               # Kanalų užimtumo registras.

    def __init__(self, real_memory,
            virtual_memory_code=None, virtual_memory_data=None):
        u""" Inicializuoja procesorių.

        + ``real_memory`` – realios mašinos atmintis.
        + ``virtual_memory_code`` – virtualios mašinos atmintis, kodo
          segmentas.
        + ``virtual_memory_data`` – virtualios mašinos atmintis, duomenų
          segmentas.
        """

        self.real_memory = real_memory
        self.virtual_memory_code = virtual_memory_code
        self.virtual_memory_data = virtual_memory_data
        self.commands = Commands()

    def set_virtual_memory(self, virtual_memory_code, virtual_memory_data):
        u""" Nurodo naudoti ``virtual_memory``, kaip virtualios atminties
        objektą.
        """

        self.virtual_memory_code = virtual_memory_code
        self.virtual_memory_data = virtual_memory_data

    def step(self):
        u""" Įvykdo vieną komandą.

        Grąžina ``True`` jei pavyko ir ``False`` kitu atveju.
        """

        print u'Žingsnis:', self.IC, self.virtual_memory_code[self.IC]
        value = self.virtual_memory_code[self.IC]
        self.IC = self.IC + 1
        self.do(**self.parse_command(value))
        return True

    def parse_command(self, value):
        u""" Iš atminties ląstelės reikšmės ``value`` atpažįsta komandos
        pavadinimą ir argumentus.

        Grąžina žodymą ``{'command': <atpažinta komanda>,
        'args': <komandos argumentų sąrašas>}``.
        """

        parts = value.split()
        command = parts[0]
        args = parts[1:]

        return {'command': command, 'args': args}

    def do(self, command, args):
        u""" Įvykdo komandą ``command`` su argumentais ``args``.
        """

        print 'command: {0} args: {1}'.format(command, args)
        self.commands[command](self, *args)

    def execute(self):
        """ Vykdo tol kol vykdosi.
        """

        while self.step():
            pass
