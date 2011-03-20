#!/usr/bin/python
# -*- coding: utf-8 -*-


from math import ceil

from pyemu.processor import Processor
from pyemu.memory import RealMemory
from pyemu.memory import BLOCK_SIZE


class RealMachine(object):
    u""" Realią mašiną simuliuojantis objektas.
    """

    def __init__(self, cell_handler=None):
        u""" Inicializuoja mašinos objektą.
        """
        
        self.real_memory = RealMemory(cell_handler)
        self.processor = Processor(self.real_memory)
        self.virtual_memory_data = None
        self.virtual_memory_code = None

    def passHandler(self, cell_handler):
        self.real_memory.SetHandler(cell_handler)

    def load_virtual_machine(
            self, file, stdin_handler=None, stdout_handler=None):
        u""" Pakrauna virtualią mašiną.
        """

        code = []
        code_size = None
        data = []
        data_size = None

        if isinstance(file, str) or isinstance(file, unicode):
            with open(file) as fp:
                code_segment = False
                data_segment = False
                for line in fp:

                    if not code_segment and not data_segment and \
                            line == 'CODE\n':
                        code_segment = True
                        continue
                    elif code_segment and not data_segment and \
                            line == 'ENDCODE\n':
                        code_segment = False
                        continue
                    elif not code_segment and not data_segment and \
                            line.startswith('DATA '):
                        data_segment = True
                        data_size = int(line[4:])
                        continue
                    elif not code_segment and data_segment and \
                            line == 'ENDDATA\n':
                        data_segment = False
                        continue

                    if code_segment:
                        code.append(line)
                    if data_segment:
                        data.append(line)
                code_size = int(ceil(float(len(code)) / BLOCK_SIZE))

            self.virtual_memory_code, self.virtual_memory_data = \
                    self.real_memory.create_virtual_memory(
                            code, code_size, data, data_size,
                            stdin_handler, stdout_handler)
            self.pager = self.virtual_memory_code.pager
            self.processor.PLR = self.virtual_memory_code.pager.PLR
            self.processor.PLBR = self.virtual_memory_code.pager.PLBR
            self.processor.set_virtual_memory(
                    self.virtual_memory_code, self.virtual_memory_data,
                    self.pager)
            self.processor.IC = 0
            # TODO: Atidaryti išorinius failus.
        else:
            # Šitas if iš esmės skirtas tam, jei kartais butų visgi 
            # nuspręsta pasinaudoti „#!/usr/bin/pyemu“ funkcionalumu.
            raise Exception('Not implemented!')
