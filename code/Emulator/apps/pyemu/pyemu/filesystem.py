#!/usr/bin/python
# -*- coding: utf-8 -*-

import os

import transaction
import persistent

from ZODB.FileStorage import FileStorage
from ZODB.DB import DB
from BTrees.OOBTree import OOBTree


DATABASE_PATH = os.path.join(
        os.path.abspath(os.path.dirname(__file__)), 'data.fs')

storage = FileStorage(DATABASE_PATH)
db = DB(storage)
connection = db.open()
root = connection.root()


class EOF(Exception):
    u""" Išimtis žyminti failo pabaigą.
    """


class File(persistent.Persistent):
    u""" Konkretus failas.
    """

    def __init__(self):
        u""" Inicijuojamas failas.
        """

        self.data = persistent.list.PersistentList()
                                        # Duomenų blokai.
        self.number = 0                 # Kiek duomenų blokų nuskaitė.

    def write(self, block):
        u""" Prideda bloką į failo pabaigą.
        """
        self.data.append(block)
        transaction.commit()

    def read(self):
        u""" Gražina bloką.

        Jei blokas yra už ribų, tai grąžina EOF išimtį.
        """

        try:
            data = self.data[number]
            self.number += 1
            return data
        except IndexError:
            raise EOF(u'Failo pabaiga')

    def reset_reader(self):
        u""" Perkrauna skaitymo skaitliuką.
        """

        self.number = 0


class FileSystem(persistent.Persistent):
    u""" Failų sistema.
    """

    def __init__(self):
        u""" Inicijuoja failų sistemą.
        """

        self.files = OOBTree()

    def create(self, name):
        u""" Sukuria failą rašymui.

        + ``name`` – failo pavadinimas.

        """

        self.files[name] = File()
        transaction.commit()
        return self.files[name]

    def open(self, name):
        u""" Atidaro egzistuojantį failą skaitymui.

        + ``name`` – failo pavadinimas.

        """

        file = self.files[name]
        file.reset_reader()
        transaction.commit()
        return file

    def delete(self, name):
        u""" Ištrina failą.

        + ``name`` – failo pavadinimas.

        """

        del self.files[name]
        transaction.commit()


if not root.has_key('fs'):
    root['fs'] = FileSystem()

file_system = root['fs']
