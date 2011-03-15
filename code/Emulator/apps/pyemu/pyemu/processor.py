#!/usr/bin/python
# -*- coding: utf-8 -*-


import inspect
import sys, traceback

from pyemu import exceptions
from pyemu.exceptions import ProgramInterrupt, StopProgram, WrongOpCode
from pyemu.registers import Register, IntegerRegister, HexRegister
from pyemu.registers import ChoiceRegister, StatusFlagRegister
from pyemu.registers import hex_to_int, set_descriptor


def convert_file_args(x, y):
    try:
        x = int(x)
        y = hex_to_int(y)
    except ValueError:
        raise WrongOpCode(u'Netinkami komandos argumentai.')
    return x, y


def convert_address_arg(y):
    try:
        y = hex_to_int(y)
    except ValueError:
        raise WrongOpCode(u'Netinkami komandos argumentai.')
    return y


class ArgumentConverter(object):
    u""" Pasirūpina, kad komandai perduoti argumentai būtų
    reikiamo tipo.
    """

    def __init__(self, *signature):
        u"""

        ``*signature`` – komandos argumentų „parašas“. Kiekvienas
        argumentas gali būti iš:

        + ``a`` – argumentas hex, konvertuoja į int;
        + ``i`` – sveikasis skaičius, konvertuoja į int;
        + ``?`` – bet koks argumentas, nekonvertuojamas.

        """

        self.signature = signature

    def __call__(self, fn):
        u""" „Dekoravimas“.

        + ``fn`` – dekoruojamoji funkcija.
        """

        def convert(proc, *args):
            u""" Funkcija, kuri konvertuoja komandų argumentus.
            """

            if len(args) != len(self.signature):
                raise WrongOpCode(
                        u'Netinkamas argumentų kiekis.')
            clean_args = []
            for arg, arg_type in zip(args, self.signature):
                try:
                    if arg_type == 'a':
                        clean_args.append(hex_to_int(arg))
                    elif arg_type == 'i':
                        clean_args.append(int(arg))
                    elif arg_type == '?':
                        clean_args.append(arg)
                    else:
                        raise TypeError(
                                u'Blogas komandos parašas.')
                except ValueError:
                    raise WrongOpCode(
                            u'Netinkami komandos argumentai.')
            return fn(proc, *clean_args)

        return convert


def check_integer_result(proc, size, x):
    try:
        Register(size).value = x
    except exceptions.ValueError:
        proc.SF.CF = 1
        x = int(str(x)[:8])
    else:
        proc.SF.CF = 0
    try:
        IntegerRegister(size).value = x
    except exceptions.ValueError:
        proc.SF.OF = 1
        x = '{0:+}'.format(x)[:8]
    else:
        proc.SF.OF = 0
    return x


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

        try:
            return self.commands[command]
        except KeyError:
            raise WrongOpCode(command)

    @staticmethod
    @ArgumentConverter('a')
    def LR1(proc, x):
        proc.R1 = proc.virtual_memory_data[x]

    @staticmethod
    @ArgumentConverter('a')
    def LR2(proc, x):
        proc.R2 = proc.virtual_memory_data[x]

    @staticmethod
    @ArgumentConverter('a')
    def SR1(proc, x):
        proc.virtual_memory_data[x] = proc.R1

    @staticmethod
    @ArgumentConverter('a')
    def SR2(proc, x):
        proc.virtual_memory_data[x] = proc.R2

    @staticmethod
    def ADD(proc):
        r1 = IntegerRegister(proc.registers['R1'].size).value = proc.R1
        r2 = IntegerRegister(proc.registers['R2'].size).value = proc.R2
        proc.R1 = check_integer_result(
                proc, proc.registers['R1'].size, int(r1) + int(r2))

    @staticmethod
    @ArgumentConverter('a')
    def ADDM(proc, x):
        r1 = IntegerRegister(proc.registers['R1'].size).value = proc.R1
        r2 = IntegerRegister(proc.registers['R2'].size).value = proc.R2
        proc.virtual_memory_data[x] = check_integer_result( 
                proc, proc.registers['R1'].size, int(r1) + int(r2))

    @staticmethod
    def SUB(proc):
        r1 = IntegerRegister(proc.registers['R1'].size).value = proc.R1
        r2 = IntegerRegister(proc.registers['R2'].size).value = proc.R2
        proc.R1 = check_integer_result(
                proc, proc.registers['R1'].size, int(r1) - int(r2))

    @staticmethod
    @ArgumentConverter('a')
    def SUBM(proc, x):
        r1 = IntegerRegister(proc.registers['R1'].size).value = proc.R1
        r2 = IntegerRegister(proc.registers['R2'].size).value = proc.R2
        proc.virtual_memory_data[x] = check_integer_result(
                proc, proc.registers['R1'].size, int(r1) - int(r2))

    @staticmethod
    def DIV(proc):
        r1 = IntegerRegister(proc.registers['R1'].size).value = proc.R1
        r2 = IntegerRegister(proc.registers['R2'].size).value = proc.R2
        proc.R1 = check_integer_result(
                proc, proc.registers['R1'].size, int(r1) / int(r2))
        proc.R2 = check_integer_result(
                proc, proc.registers['R2'].size, int(r1) % int(r2))

    @staticmethod
    def MUL(proc):
        r1 = IntegerRegister(proc.registers['R1'].size).value = proc.R1
        r2 = IntegerRegister(proc.registers['R2'].size).value = proc.R2
        proc.R1 = check_integer_result(
                proc, proc.registers['R1'].size, int(r1) * int(r2))

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
    @ArgumentConverter('a')
    def JMP(proc, x):
        proc.IC = x

    @staticmethod
    @ArgumentConverter('a')
    def JE(proc, x):
        if proc.SF.ZF == 1:
            proc.IC = x

    @staticmethod
    @ArgumentConverter('a')
    def JA(proc, x):
        if proc.SF.ZF == 0 and proc.SF.SF == 0:
            proc.IC = x

    @staticmethod
    @ArgumentConverter('a')
    def JNB(proc, x):
        if proc.SF.SF == 0:
            proc.IC = x

    @staticmethod
    @ArgumentConverter('a')
    def JB(proc, x):
        if proc.SF.SF == 1:
            proc.IC = x

    @staticmethod
    @ArgumentConverter('a')
    def JNA(proc, x):
        if proc.SF.ZF == 1 or proc.SF.SF == 1:
            proc.IC = x

    @staticmethod
    @ArgumentConverter('a')
    def JC(proc, x):
        if proc.SF.CF == 1:
            proc.IC = x

    @staticmethod
    @ArgumentConverter('a')
    def JO(proc, x):
        if proc.SF.OF == 1:
            proc.IC = x

    @staticmethod
    @ArgumentConverter('i', 'a')
    def PD(proc, x, y):
        proc.pager.file_write(
                x, proc.virtual_memory_data.get_block(y))

    @staticmethod
    @ArgumentConverter('a')
    def PDR(proc, y):
        r1 = IntegerRegister(proc.registers['R1'].size).value = proc.R1
        proc.pager.file_write(
                int(r1), proc.virtual_memory_data.get_block(y))

    @staticmethod
    @ArgumentConverter('i', 'a')
    def GD(proc, x, y):
        proc.virtual_memory_data.set_block(y, proc.pager.file_read(x))

    @staticmethod
    @ArgumentConverter('a')
    def GDR(proc, y):
        r1 = IntegerRegister(proc.registers['R1'].size).value = proc.R1
        proc.virtual_memory_data.set_block(
                hex_to_int(y), proc.pager.file_read(int(r1)))

    @staticmethod
    @ArgumentConverter('?', 'a')
    def FO(proc, x, y):
        name = proc.virtual_memory_data[y]
        if x == 'r':
            proc.R1 = proc.pager.file_open(name)
        elif x == 'w':
            proc.R1 = proc.pager.file_create(name)
        else:
            raise exceptions.ValueError(
                    u'Neteisingas failo atidarymo rėžimas.')

    @staticmethod
    @ArgumentConverter('a')
    def FC(proc, x):
        proc.pager.file_close(x)

    @staticmethod
    def FCR(proc):
        r1 = IntegerRegister(proc.registers['R1'].size).value = proc.R1
        proc.pager.file_close(int(r1))

    @staticmethod
    @ArgumentConverter('a')
    def FD(proc, x):
        name = proc.virtual_memory_data[x]
        proc.pager.file_delete(name)

    #@staticmethod
    #def <++>(proc<++>):
        #<++>

    @staticmethod
    def HALT(proc):
        raise StopProgram('HALT')


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

    def _step(self):
        """ Įvykdo vieną komandą.

        Jei pavyko grąžina ``True``, kitu atveju išmeta išimtį.
        """

        value = self.virtual_memory_code[self.IC]
        self.IC = self.IC + 1
        self.do(**self.parse_command(value))

        return True

    def step(self):
        u""" Įvykdo vieną komandą.

        Grąžina ``True`` jei pavyko ir ``False`` kitu atveju.
        """

        print u'Žingsnis:'.encode('utf-8'),
        print self.IC, self.virtual_memory_code[self.IC]
        old_ic = self.IC
        try:
            self._step()
        except ProgramInterrupt:
            self.PI = 1
            self.IC = old_ic
            return False
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

        print 'IC: {2:3} R1: {3} R2 {4} command: {0} args: {1}'.format(
                command, args, self.IC, self.R1, self.R2)
        try:
            self.commands[command](self, *args)
        except TypeError, e:
            raise WrongOpCode(u'Netinkami argumentai komandai.')

    def execute(self):
        """ Vykdo tol kol vykdosi.
        """

        while self.step():
            pass
