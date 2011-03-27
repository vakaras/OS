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
extern void gdt_flush(u32int);
extern void idt_flush(u32int);


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


// Atributai.
private:

// Metodai.
public:

// Pagalbinės struktūros.

  struct Entry {
    u16int  base_lo;
    u16int  sel;
    u8int   always0;
    u8int   flags;
    u16int  base_hi;
    } __attribute__((packed));

  struct Pointer {
    u16int limit;
    u32int base;
    } __attribute__((packed));

  };


// Šios funkcijos (pertraukimai) realizuotos interrupt.s faile.
extern void isr0 ();
extern void isr1 ();
extern void isr2 ();
extern void isr3 ();
extern void isr4 ();
extern void isr5 ();
extern void isr6 ();
extern void isr7 ();
extern void isr8 ();
extern void isr9 ();
extern void isr10();
extern void isr11();
extern void isr12();
extern void isr13();
extern void isr14();
extern void isr15();
extern void isr16();
extern void isr17();
extern void isr18();
extern void isr19();
extern void isr20();
extern void isr21();
extern void isr22();
extern void isr23();
extern void isr24();
extern void isr25();
extern void isr26();
extern void isr27();
extern void isr28();
extern void isr29();
extern void isr30();
extern void isr31();

#endif
