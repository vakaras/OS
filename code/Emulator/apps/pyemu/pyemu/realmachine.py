#!/usr/bin/python
# -*- coding: utf-8 -*-


from math import ceil

from pyemu.processor import Processor
from pyemu.memory import RealMemory
from pyemu.memory import BLOCK_SIZE


class RealMachine(object):
    u""" Realią mašiną simuliuojantis objektas.
    """

    def __init__(self):
        u""" Inicializuoja mašinos objektą.
        """

        self.real_memory = RealMemory()
        self.processor = Processor(self.real_memory)
        self.virtual_memory_data = None
        self.virtual_memory_code = None

    def load_virtual_machine(self, file):
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
                            line == 'CODE':
                        code_segment = True
                    elif code_segment and not data_segment and \
                            line == 'ENDCODE':
                        code_segment = False
                    elif not code_segment and not data_segment and \
                            line.startswith('DATA '):
                        data_segment = True
                        data_size = int(line[4:])
                    elif not code_segment and data_segment and \
                            line == 'ENDDATA':
                        data_segment = False

                    if code_segment:
                        code.append(line)
                    if data_segment:
                        data.append(line)
                code_size = int(ceil(float(len(code)) / BLOCK_SIZE))

            self.virtual_memory_code, self.virtual_memory_data = \
                    self.real_memory.create_virtual_memory(
                            code, code_size, data, data_size)
            self.processor.PLR = self.virtual_memory_code.pager.PLR
            self.processor.PLBR = self.virtual_memory_code.pager.PLBR
            self.processor.set_virtual_memory(
                    self.virtual_memory_code, self.virtual_memory_data)
            # TODO: Atidaryti išorinius failus.
        else:
            raise Exception('Not implemented!')

