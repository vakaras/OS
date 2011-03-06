#!/usr/bin/python
# -*- coding: utf-8 -*-


from pyemu.registers import Register, IntegerRegister, HexRegister
from pyemu.registers import ChoiceRegister, StatusFlagRegister

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

    def set_virtual_memory(self, virtual_memory):
        u""" Nurodo naudoti ``virtual_memory``, kaip virtualios atminties
        objektą.
        """

        self.virtual_memory = virtual_memory

    def step(self):
        """ Įvykdo vieną komandą.

        Grąžina ``True`` jei pavyko ir ``False`` kitu atveju.
        """
        raise Exception('Not implemented!')

    def execute(self):
        """ Vykdo tol kol vykdosi.
        """

        while self.step():
            pass
