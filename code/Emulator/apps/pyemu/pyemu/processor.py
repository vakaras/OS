#!/usr/bin/python
# -*- coding: utf-8 -*-


import inspect

from pyemu.registers import Register, IntegerRegister, HexRegister
from pyemu.registers import ChoiceRegister, StatusFlagRegister
from pyemu.registers import hex_to_int, set_descriptor


class StopProgram(Exception):
    u""" Išimtis nurodanti, kad reikia baigti programos darbą.
    """


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
        proc.virtual_memory_data[hex_to_int(x)] = proc.R1

    @staticmethod
    def SR2(proc, x):
        proc.virtual_memory_data[hex_to_int(x)] = proc.R2

    @staticmethod
    def ADD(proc):
        r1 = IntegerRegister(proc.registers['R1'].size).value = proc.R1
        r2 = IntegerRegister(proc.registers['R2'].size).value = proc.R2
        proc.R1 = int(r1) + int(r2)
        # FIXME: Kaip turi suformuoti SF požymius?
        # TODO: Išimtys turi sukelti pertraukimus.

    @staticmethod
    def ADDM(proc, x):
        r1 = IntegerRegister(proc.registers['R1'].size).value = proc.R1
        r2 = IntegerRegister(proc.registers['R2'].size).value = proc.R2
        proc.virtual_memory_data[hex_to_int(x)] = int(r1) + int(r2)
        # FIXME: Kaip turi suformuoti SF požymius?

    @staticmethod
    def SUB(proc):
        r1 = IntegerRegister(proc.registers['R1'].size).value = proc.R1
        r2 = IntegerRegister(proc.registers['R2'].size).value = proc.R2
        proc.R1 = int(r1) - int(r2)
        # FIXME: Kaip turi suformuoti SF požymius?

    @staticmethod
    def SUBM(proc, x):
        r1 = IntegerRegister(proc.registers['R1'].size).value = proc.R1
        r2 = IntegerRegister(proc.registers['R2'].size).value = proc.R2
        proc.virtual_memory_data[hex_to_int(x)] = int(r1) - int(r2)
        # FIXME: Kaip turi suformuoti SF požymius?

    @staticmethod
    def DIV(proc):
        r1 = IntegerRegister(proc.registers['R1'].size).value = proc.R1
        r2 = IntegerRegister(proc.registers['R2'].size).value = proc.R2
        proc.R1 = int(r1) / int(r2)
        proc.R2 = int(r1) % int(r2)
        # FIXME: Kaip turi suformuoti SF požymius?

    @staticmethod
    def MUL(proc):
        r1 = IntegerRegister(proc.registers['R1'].size).value = proc.R1
        r2 = IntegerRegister(proc.registers['R2'].size).value = proc.R2
        proc.R1 = int(r1) * int(r2)
        # FIXME: Kaip turi suformuoti SF požymius?

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
    def JMP(proc, x):
        proc.IC = x

    @staticmethod
    def JE(proc, x):
        if proc.SF.ZF == 1:
            proc.IC = x

    @staticmethod
    def JA(proc, x):
        if proc.SF.ZF == 0 and proc.SF.SF == 0:
            proc.IC = x

    @staticmethod
    def JNB(proc, x):
        if proc.SF.SF == 0:
            proc.IC = x

    @staticmethod
    def JB(proc, x):
        if proc.SF.SF == 1:
            proc.IC = x

    @staticmethod
    def JNA(proc, x):
        if proc.SF.ZF == 1 or proc.SF.SF == 1:
            proc.IC = x

    @staticmethod
    def JC(proc, x):
        if proc.SF.CF == 1:
            proc.IC = x

    @staticmethod
    def JO(proc, x):
        if proc.SF.OF == 1:
            proc.IC = x

    @staticmethod
    def PD(proc, x, y):
        proc.pager.file_write(
                int(x), proc.virtual_memory_data.get_block(hex_to_int(y)))

    @staticmethod
    def GD(proc, x, y):
        proc.virtual_memory_data.set_block(
                hex_to_int(y), proc.pager.file_read(int(x)))

    @staticmethod
    def HALT(proc):
        raise StopProgram('HALT')

    #@staticmethod
    #def <++>(proc<++>):
        #<++>



class Processor(object):
    u""" Realios mašinos procesorius.
    """

    def __init__(self, real_memory,
            virtual_memory_code=None, virtual_memory_data=None,
            pager=None):
        u""" Inicializuoja procesorių.

        + ``real_memory`` – realios mašinos atmintis.
        + ``virtual_memory_code`` – virtualios mašinos atmintis, kodo
          segmentas.
        + ``virtual_memory_data`` – virtualios mašinos atmintis, duomenų
          segmentas.
        + ``pager`` – puslapiavimo mechanizmas.
        """

        self.real_memory = real_memory
        self.virtual_memory_code = virtual_memory_code
        self.virtual_memory_data = virtual_memory_data
        self.commands = Commands()
        self.pager = pager

        self.registers = {
                'R1': Register(),       # Žodžio ilgio bendro naudojimo 
                                        # registras.
                'R2': Register(),       # Žodžio ilgio bendro naudojimo 
                                        # registras.
                'SF': StatusFlagRegister(),
                                        # Aritmetinių operacijų loginės
                                        # reikšmės.
                'IC' : HexRegister(3),  # Nurodo vykdomos komandos adresą
                                        # atmintyje.
                'PLR' : HexRegister(2), # Puslapių lentelės bloko adresas.
                'PLBR' : HexRegister(2),# Puslapių lentelės pirmojo baito
                                        # adresas bloke.
                'MODE' : ChoiceRegister(['N', 'S']),
                                        # Nurodo procesoriaus darbo rėžimą:
                                        # „N“ – naudotojo, 
                                        # „S“ – supervizoriaus.

                'PI' : ChoiceRegister([0, 1]),
                                        # Programiniai pertraukimai.
                'SI' : ChoiceRegister([0, 1]),
                                        # Supervizoriniai pertraukimai.
                'TI' : ChoiceRegister([0, 1]),
                                        # Laikrodžio pertraukimai.
                'IOI' : HexRegister(1),
                                        # Įvedimo / išvedimo pertraukimai.
                'CHST' : HexRegister(1),
                                        # Kanalų užimtumo registras.
                }
        for name, register in self.registers.items():
            set_descriptor(self, name, register)


    def set_virtual_memory(
            self, virtual_memory_code, virtual_memory_data, pager):
        u""" Nurodo naudoti ``virtual_memory``, kaip virtualios atminties
        objektą.
        """

        self.virtual_memory_code = virtual_memory_code
        self.virtual_memory_data = virtual_memory_data
        self.pager = pager

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

        try:
            while self.step():
                pass
        except StopProgram:
            return
