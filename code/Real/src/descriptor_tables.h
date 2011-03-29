/**
 * descriptor_tables.h – Aprašoma sąsaja darbui su GDT (Global Descriptor 
 * Table) ir IDT (Interrupt Descriptor Table).
 */
#ifndef DESCRIPTOR_TABLES_H
#define DESCRIPTOR_TABLES_H

#include "common.h"
#define GDT_NUMBER 5
#define IDT_NUMBER 256


// Šios funkcijos realizuotos gdt.s faile.
extern "C" void gdt_flush(u32int);
extern "C" void idt_flush(u32int);

// Šios funkcijos (pertraukimai) realizuotos interrupt.s faile.
extern "C" void isr0 ();
extern "C" void isr1 ();
extern "C" void isr2 ();
extern "C" void isr3 ();
extern "C" void isr4 ();
extern "C" void isr5 ();
extern "C" void isr6 ();
extern "C" void isr7 ();
extern "C" void isr8 ();
extern "C" void isr9 ();
extern "C" void isr10();
extern "C" void isr11();
extern "C" void isr12();
extern "C" void isr13();
extern "C" void isr14();
extern "C" void isr15();
extern "C" void isr16();
extern "C" void isr17();
extern "C" void isr18();
extern "C" void isr19();
extern "C" void isr20();
extern "C" void isr21();
extern "C" void isr22();
extern "C" void isr23();
extern "C" void isr24();
extern "C" void isr25();
extern "C" void isr26();
extern "C" void isr27();
extern "C" void isr28();
extern "C" void isr29();
extern "C" void isr30();
extern "C" void isr31();

// TODO: Išsiaiškinti.
struct Pointer {
  u16int limit;
  u32int base;
  } __attribute__((packed));


// TODO: Išsiaiškinti.
class GDT {

public:

// Pagalbinės struktūros.

  struct Entry {

    // Atributai.

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

    this->pointer.limit = sizeof(Entry) * 5 - 1;
    this->pointer.base = (u32int) (&this->entries);

    gdt_flush((u32int) &this->pointer);
    }

  };

// TODO: Išsiaiškinti.
class IDT {

public:

// Pagalbinės struktūros.

  struct Entry {

    // Atributai.

    u16int  base_lo;
    u16int  sel;
    u8int   always0;
    u8int   flags;
    u16int  base_hi;

    // Metodai.

    Entry() {
      this->base_lo = 0;
      this->sel = 0;
      this->always0 = 0;
      this->flags = 0;
      this->base_hi = 0;
      }

    void set_base(u32int base) {
      this->base_lo = base & 0xFFFF;
      this->base_hi = (base >> 16) & 0xFFFF;
      }

    void set_selector(u16int sel) {
      this->sel = sel;
      }
    
    void set_flags(u8int flags) {
      this->flags = flags;
      //this->flags = flags | 0x60;     Naudotojo rėžimas.
      }
    
    } __attribute__((packed));

// Atributai.
private:

  Entry entries[IDT_NUMBER];
  Pointer pointer;

// Metodai.
public:

  IDT() {

    //memset((u8int *)&this->entries, 0, sizeof(Entry) * 256);

    this->pointer.limit = sizeof(Entry) * 5 - 1;
    this->pointer.base = (u32int) (&this->entries);

    for (int i = 0; i < 32; i++) {
      this->entries[i].set_selector(0x08);
      this->entries[i].set_flags(0x8E);
      }
    entries[0].set_base((u32int) isr0);
    entries[1].set_base((u32int) isr1);
    entries[2].set_base((u32int) isr2);
    entries[3].set_base((u32int) isr3);
    entries[4].set_base((u32int) isr4);
    entries[5].set_base((u32int) isr5);
    entries[6].set_base((u32int) isr6);
    entries[7].set_base((u32int) isr7);
    entries[8].set_base((u32int) isr8);
    entries[9].set_base((u32int) isr9);
    entries[10].set_base((u32int) isr10);
    entries[11].set_base((u32int) isr11);
    entries[12].set_base((u32int) isr12);
    entries[13].set_base((u32int) isr13);
    entries[14].set_base((u32int) isr14);
    entries[15].set_base((u32int) isr15);
    entries[16].set_base((u32int) isr16);
    entries[17].set_base((u32int) isr17);
    entries[18].set_base((u32int) isr18);
    entries[19].set_base((u32int) isr19);
    entries[20].set_base((u32int) isr20);
    entries[21].set_base((u32int) isr21);
    entries[22].set_base((u32int) isr22);
    entries[23].set_base((u32int) isr23);
    entries[24].set_base((u32int) isr24);
    entries[25].set_base((u32int) isr25);
    entries[26].set_base((u32int) isr26);
    entries[27].set_base((u32int) isr27);
    entries[28].set_base((u32int) isr28);
    entries[29].set_base((u32int) isr29);
    entries[30].set_base((u32int) isr10);
    entries[31].set_base((u32int) isr11);

    idt_flush((u32int) &this->pointer);
    }

  };

#endif
