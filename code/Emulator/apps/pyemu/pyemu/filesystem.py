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

    def write(self, block):
        u""" Prideda bloką į failo pabaigą.
        """
        self.data.append(block)

    def read(self, number):
        u""" Gražina nurodytąjį bloką.

        Jei blokas yra už ribų, tai grąžina EOF išimtį.
        """

        try:
            return self.data[number]
        except IndexError:
            raise EOF(u'Failo pabaiga')


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
        return self.files[name]

    def open(self, name):
        u""" Atidaro egzistuojantį failą skaitymui.

        + ``name`` – failo pavadinimas.

        """

        return self.files[name]

    def delete(self, name):
        u""" Ištrina failą.

        + ``name`` – failo pavadinimas.

        """

        del self.files[name]


if not root.has_key('fs'):
    root['fs'] = FileSystem()

file_system = root['fs']
