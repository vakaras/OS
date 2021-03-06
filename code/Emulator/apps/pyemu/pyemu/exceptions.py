#!/usr/bin/python
# -*- coding: utf-8 -*-


class Interrupt(Exception):
    u""" Klasė pertraukimams fiksuoti.
    """


class ProgramInterrupt(Interrupt):
    u""" Klasė programiniams pertraukimams fiksuoti.

    Iš esmės tai yra visos klaidos, kurios kyla dėl klaidų emuliuojamoje
    programoje.
    """


class WrongOpCode(ProgramInterrupt):
    u""" Klasė neegzistuojančio operacijos kodo klaidoms fiksuoti.
    """


class BadAddress(ProgramInterrupt):
    u""" Klasė blogo adreso klaidoms fiksuoti.
    """


class ValueError(ProgramInterrupt):
    u""" Klasė netinkamoms reikšmėms žymėti.
    """


class StopProgram(ProgramInterrupt):
    u""" Klasė HALT instrukcijoms fiksuoti.
    """


class EndOfFile(ProgramInterrupt):
    u""" Klasė failo pabaigai žymėti.
    """

class WrongFileDescriptor(ProgramInterrupt):
    u""" Klasė failų descriptorius klaidoms žymėti.
    """

class ReadingWriteOnlyFileError(ProgramInterrupt):
    u""" Klasė skaitymo iš rašymui atidaryto failo klaidai pažymėti.
    """

class WritingReadOnlyFileError(ProgramInterrupt):
    u""" Klasė rašymo į skaitymui atidaryto failo klaidai pažymėti.
    """
