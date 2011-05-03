#ifndef IDT_H
#define IDT_H

#include "types.h"
#include "monitor.h"
#include "primitives.h"

#define IDT_NUMBER 64

extern "C" IntPtr isr_table[IDT_NUMBER];
extern "C" void idt_flush(u64int);


class IDT{

public:
  
  struct Idtr_pointer
  {
    u16int    limit;
    u64int    base;            // The address of 
    // the first element in idt_entry_t array.
  } __attribute__((packed));
  
  // A struct describing an interrupt gate.
  struct Interrupt_gate
  {
    u16int offset_0_15;       // The lower 16 bits of the address to jump
    // to when this interrupt fires. (base_lo)
    u16int selector;          // Kernel segment selector.
    Byte  zero;               // This must always be zero.
    Byte  flags;              // More flags. 0xE - IntGate 0xF - TrapGate
    u16int offset_16_31;      // The upper 16 bits of the address 
    // to jump to. (base_hi)
    
    u32int offset_32_63;       // Higher half offset.
    u32int reserved;           // System reserved bytes
    
    Interrupt_gate() {
      this->offset_0_15 = 0;
      this->selector = 0x18;
      this->zero = (Byte) 0;
      this->flags = 0x8E;
      this->offset_16_31 = 0;
      this->offset_32_63 = 0;
      this->reserved = 0;
    }
    
    void set_flags(Byte flags){
      this->flags = flags;
    }
    
    void set_offsets(u64int function_handler){
      this->offset_0_15 = (u16int)(function_handler & 0xFFFF);
      this->offset_16_31 = (u16int)((function_handler >> 16) & 0xFFFF);
      this->offset_32_63 = (u32int)((function_handler >> 32) & 0xFFFFFFFF);
    }
    
    void set_selector(u16int selector){
      this->selector = selector;
    }
  } __attribute__((packed));
  
  //typedef void (*isr_t)(struct context_s *);
  
private:
  Idtr_pointer idtr;
  Interrupt_gate int_gates[IDT_NUMBER];
//   Monitor *monitor;
  
  void install_idt(u64int idt_ptr)
  {
    asm volatile("lidt (%0);"::"r"(idt_ptr):"memory");  
  }
  
  void irq_remap()
  {
    send_byte(0x20, 0x11);
    send_byte(0xA0, 0x11);
    send_byte(0x21, 0x20);
    send_byte(0xA1, 0x28);
    send_byte(0x21, 0x04);
    send_byte(0xA1, 0x02);
    send_byte(0x21, 0x01);
    send_byte(0xA1, 0x01);
    
    send_byte(0x21, 0x00); // unmask all ISRs
    send_byte(0xA1, 0x00); // unmask all ISRs
  }

public:
  
//   void set_monitor(Monitor *monitor){
//     this->monitor = monitor;
//   }
  
  IDT(){
    this->irq_remap();
    
    for(u64int i = 0; i < IDT_NUMBER; i++){
      this->int_gates[i].set_offsets(isr_table[i]);
    }
    
    this->idtr.limit = sizeof(Interrupt_gate) * IDT_NUMBER - 1;
    this->idtr.base = (u64int)&this->int_gates;
    
    this->install_idt((u64int) &this->idtr);
  }
  
  void print_debug_info(Monitor *monitor) {
    
    monitor->write_string("\n\nIDT debug information.");
    monitor->write_string("\n64 bit unsigned zero:  ");
    monitor->write_hex((u64int) 0);
    monitor->write_string("\nGates start address:   ");
    monitor->write_hex((u64int) this->int_gates);
    monitor->write_string("\nIDTR address:          ");
    monitor->write_hex((u64int) &this->idtr);
    monitor->write_string("\nIdtr.limit:            ");
    monitor->write_hex(this->idtr.limit);
    monitor->write_string("\nIdtr.base:             ");
    monitor->write_hex(this->idtr.base);
    monitor->write_string("\nInterrupt_gate dydis:  ");
    monitor->write_dec((u32int)sizeof(Interrupt_gate));
    
  }
  
  
};

#endif
