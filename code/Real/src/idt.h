#ifndef IDT_H
#define IDT_H

#include "types.h"

#define IDT_NUMBER 256


// Ši funkcija realizuota asm_primitives.s faile.
extern "C" void idt_flush(u64int);

extern "C" void isr0();
extern "C" void isr1();
extern "C" void isr2();
extern "C" void isr3();
extern "C" void isr4();


// Aprašymas 251 puslapyje.
class IDT {

public:

// Pagalbinės struktūros.

  // Rodyklė į IDT lentelę.
  // FIXME: Galimai šlapias kodas.
  struct Pointer {
    u16int limit;
    u64int base;
    } __attribute__((packed));

  struct Entry {

    // TODO: Drivers/pic.c
    // Atributai.

    u16int  offset_0_15;
    u16int  selector;
    u8int   zero;
    u8int   flags;
    u16int  offset_16_31;
    u32int  offset_32_63;
    u32int  reserved;

    // Metodai.

    Entry() {
      this->offset_0_15 = 0;
      this->selector = 0;
      this->zero = 0;
      this->flags = 0;
      this->offset_16_31 = 0;
      this->offset_32_63 = 0;
      this->reserved = 0;
      }

    void set_selector(u16int selector) {
      this->selector = selector;
      }
    
    void set_offset(u64int offset) {
      this->offset_0_15 = (u16int)(offset & 0xFFFF);
      this->offset_16_31 = (u16int)((offset >> 16) & 0xFFFF);
      this->offset_32_63 = (u32int)((offset >> 32) & 0xFFFFFFFF);
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

    this->pointer.limit = sizeof(Entry) * IDT_NUMBER - 1;
    this->pointer.base = (u64int) (&this->entries);

    for (int i = 0; i < 32; i++) {
      this->entries[i].set_selector(0x18);
      this->entries[i].set_flags(0x8E);
      }
    this->entries[0].set_offset((u64int) isr0);
    this->entries[1].set_offset((u64int) isr1);
    this->entries[2].set_offset((u64int) isr2);
    this->entries[3].set_offset((u64int) isr3);
    this->entries[4].set_offset((u64int) isr4);
    //this->entries[5].set_offset((u64int) isr5);
    //this->entries[6].set_offset((u64int) isr6);
    //this->entries[7].set_offset((u64int) isr7);
    //this->entries[8].set_offset((u64int) isr8);
    //this->entries[9].set_offset((u64int) isr9);
    //entries[10].set_base((u32int) isr10);
    //entries[11].set_base((u32int) isr11);
    //entries[12].set_base((u32int) isr12);
    //entries[13].set_base((u32int) isr13);
    //entries[14].set_base((u32int) isr14);
    //entries[15].set_base((u32int) isr15);
    //entries[16].set_base((u32int) isr16);
    //entries[17].set_base((u32int) isr17);
    //entries[18].set_base((u32int) isr18);
    //entries[19].set_base((u32int) isr19);
    //entries[20].set_base((u32int) isr20);
    //entries[21].set_base((u32int) isr21);
    //entries[22].set_base((u32int) isr22);
    //entries[23].set_base((u32int) isr23);
    //entries[24].set_base((u32int) isr24);
    //entries[25].set_base((u32int) isr25);
    //entries[26].set_base((u32int) isr26);
    //entries[27].set_base((u32int) isr27);
    //entries[28].set_base((u32int) isr28);
    //entries[29].set_base((u32int) isr29);
    //entries[30].set_base((u32int) isr10);
    //entries[31].set_base((u32int) isr11);

    // TODO: Įrašyti realų adresą.
    idt_flush((u64int) &this->pointer);
    }

  };

#endif
