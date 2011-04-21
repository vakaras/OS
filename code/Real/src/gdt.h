#ifndef GDT_H
#define GDT_H

#include "types.h"
#include "monitor.h"

#define GDT_NUMBER 5


// Ši funkcija realizuota asm_primitives.s faile.
extern "C" void gdt_flush(u64int);


/**
 * GDT – Global Descriptor Table.
 *
 * Kadangi šis metodas yra nenaudojamas, tai tiesiog inicijuojama,
 * kad adresai yra nuo 0x0000000000000000 iki 0xFFFFFFFFFFFFFFFF.
 *
 * Aprašymai, kas tai yra:
 *
 * http://en.wikipedia.org/wiki/Global_Descriptor_Table
 * http://wiki.osdev.org/GDT_Tutorial
 *
 * Taip pat verta dėmesio, kas yra „Canonical form address“:
 * http://en.wikipedia.org/wiki/Intel_64#Canonical_form_addresses
 */
class GDT {

public:

// Pagalbinės struktūros.

  // Rodyklė į GDT lentelę.
  // FIXME: Galimai šlapias kodas.
  struct Pointer {
    u16int limit;
    u64int base;
    } __attribute__((packed));

  // Lentelės įrašas.
  struct Entry {

    // Atributai.

    // FIXME: Būtina perdaryti 64 bitų architektūrai.
    u16int  limit_low;
    u16int  base_low;
    u8int   base_middle;
    u8int   access;
    u8int   granularity;
    u8int   base_high;

    // Metodai.
    Entry() {
      this->limit_low = 0;
      this->base_low = 0;
      this->base_middle = 0;
      this->access = 0;
      this->granularity = 0;
      this->base_high = 0;
      }
    
    void set_access(u8int access) {
      this->access = access;
      }

    void set_base(u32int base) {
      this->base_low = (base & 0xFFFF);
      this->base_middle = (base >> 16) & 0xFF;
      this->base_high = (base >> 24) & 0xFF;
      }

    void set_limit_granularity(u32int limit, u8int granularity) {
      this->limit_low = (limit & 0xFFFF);
      this->granularity = (limit >> 16) & 0x0F;
      this->granularity |= granularity & 0xF0;
      }
    
    } __attribute__((packed));

// Atributai.

private:

  Entry entries[GDT_NUMBER];
  Pointer pointer;

// Metodai.
public:

  GDT() {

    for (int i = 1; i < GDT_NUMBER; i++) {
      this->entries[i].set_limit_granularity(0xFFFFFFFF, 0xCF);
      }

    this->entries[1].set_access(0x9A);
    this->entries[2].set_access(0x92);
    this->entries[3].set_access(0xFA);
    this->entries[4].set_access(0xF2);

    this->pointer.limit = sizeof(Entry) * GDT_NUMBER - 1;
    this->pointer.base = (u64int) (&this->entries);

    gdt_flush((u64int) &this->pointer);
    }

  void print_debug_info(Monitor *monitor) {

    monitor->clear();
    monitor->write_string("GDT debug information.");
    monitor->write_string("\n64 bit unsigned zero:  ");
    monitor->write_hex((u64int) 0);
    monitor->write_string("\nEntries start address: ");
    monitor->write_hex((u64int) this->entries);
    monitor->write_string("\nPointer address:       ");
    monitor->write_hex((u64int) &this->pointer);
    monitor->write_string("\nPointer.limit:         ");
    monitor->write_hex(this->pointer.limit);
    monitor->write_string("\nPointer.base:          ");
    monitor->write_hex(this->pointer.base);
    
    }

  };

#endif
