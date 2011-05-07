#ifndef IDT_H
#define IDT_H 1

#include "types.h"
#include "monitor.h"
#include "primitives.h"

#define IDT_NUMBER 64

#define PHYSICAL_ADDRESS_START 0x00000000001fa000
#define VIRTUAL_ADDRESS_START 0xffff800000000000
#define FIX_ADDRESS(a) ((a) - \
VIRTUAL_ADDRESS_START + PHYSICAL_ADDRESS_START)

extern "C" IntPtr isr_table[IDT_NUMBER];

#define EXCEPTION_FAULT     1
#define EXCEPTION_TRAP      2
#define EXCEPTION_INTERRUPT 3
#define EXCEPTION_ABORT     4

static struct exception
{
  const char * mnemonic;
  const char * description;
  const Byte type;
  const Boolean error_code;
} exception[32] =
{
  { "DE", "Divide Error", EXCEPTION_FAULT, false },
  { "DB", "RESERVED", EXCEPTION_TRAP, false },
  { "--", "NMI Interrupt", EXCEPTION_INTERRUPT, false },
  { "BP", "Breakpoint", EXCEPTION_TRAP, false },
  { "OF", "Overflow", EXCEPTION_TRAP, false },
  { "BR", "BOUND Range Exceeded", EXCEPTION_FAULT, false },
  { "UD", "Invalid Opcode (Undefined Opcode)", EXCEPTION_FAULT, false },
  { "NM", "Device Not Available (No Math Coprocessor)", EXCEPTION_FAULT, false },
  { "DF", "Double Fault", EXCEPTION_ABORT, true },
  { "--", "Coprocessor Segment Overrun (reserved)", EXCEPTION_FAULT, false },
  { "TS", "Invalid TSS", EXCEPTION_FAULT, true },
  { "NP", "Segment Not Present", EXCEPTION_FAULT, true },
  { "SS", "Stack-Segment Fault", EXCEPTION_FAULT, true },
  { "GP", "General Protection", EXCEPTION_FAULT, true },
  { "PF", "Page Fault", EXCEPTION_FAULT, true },
  { "--", "(Intel reserved. Do not use.)", EXCEPTION_FAULT, false },
  { "MF", "x87 FPU Floating-Point Error (Math Fault)", EXCEPTION_FAULT, false },
  { "AC", "Alignment Check", EXCEPTION_FAULT, true },
  { "MC", "Machine Check", EXCEPTION_ABORT, false },
  { "XM", "SIMD Floating-Point Exception", EXCEPTION_FAULT, false },
  { "--", "Intel reserved. Do not use.", EXCEPTION_FAULT, false },
  { "--", "Intel reserved. Do not use.", EXCEPTION_FAULT, false },
  { "--", "Intel reserved. Do not use.", EXCEPTION_FAULT, false },
  { "--", "Intel reserved. Do not use.", EXCEPTION_FAULT, false },
  { "--", "Intel reserved. Do not use.", EXCEPTION_FAULT, false },
  { "--", "Intel reserved. Do not use.", EXCEPTION_FAULT, false },
  { "--", "Intel reserved. Do not use.", EXCEPTION_FAULT, false },
  { "--", "Intel reserved. Do not use.", EXCEPTION_FAULT, false },
  { "--", "Intel reserved. Do not use.", EXCEPTION_FAULT, false },
  { "--", "Intel reserved. Do not use.", EXCEPTION_FAULT, false },
  { "--", "Intel reserved. Do not use.", EXCEPTION_FAULT, false },
  { "--", "Intel reserved. Do not use.", EXCEPTION_FAULT, false },
};


//==========================================================================
// Klaviatūros išdėstymai:
// - lowercase: paprasti pirminiai simboliai
// - uppercase: simboliai, gaunami laikant paspaustą LShift arba RShift
//
// Šaltiniai:
// - http://www.win.tue.nl/~aeb/linux/kbd/scancodes-10.html
// - Mano klaviatūros išdėstymas o.O
//
// TODO:
// - Sutvarkyti escaped chars
// - Išanalizuoti Enter veikimą (dabar nustatyta Enter = 13, 
//                               LShift/RShift+Enter = "\n")
// - Padaryti Caps Lock
// - Padaryti LEDs veikimą
//
// P.S. Sorry už ilgas eilutes
//==========================================================================



char lowercase[] =
{
  0, 0 /*esc*/, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', 8 /*backspace*/,
  '\t' /*tab*/, 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']',  13 /*enter*/,
  0 /*ctrl*/, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', 0 /*lshift*/, '\\',
  'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/',
  0 /*rshift*/, 0 /*sysreq*/,  0 /*alt*/, ' ',
  0 /*capslock*/, 0 /*F1*/,  0 /*F2*/, 0 /*F3*/,
  0 /*F4*/, 0 /*F5*/, 0 /*F6*/,  0 /*F7*/, 0 /*F8*/,
  0 /*F9*/,  0 /*F10*/, 0 /*numlock*/, 0 /*scrolllock*/, 
  0 /*home*/, 0 /*uarrow*/, 0 /*pup*/, 0 /*numminus*/,
  0 /*larrow*/, 0 /*num5*/, 0 /*rarrow*/, 0 /*numplus*/, 0 /*end*/,
  0 /*darrow*/, 0 /*pdown*/, 0 /*ins*/, 0 /*del*/, 0, 0, '\\', 
  0 /*F11*/, 0 /*F12*/
};

char uppercase[] =
{
  0, 0 /*esc*/, '!',  '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', 8 /*backspace*/,
  '\t' /*tab*/, 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n',
  0 /*ctrl*/, 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '~', 0 /*lshift*/, '|',
  'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?',
  0 /*rshift*/, 0 /*sysreq*/, 0 /*alt*/, ' ', 
  0 /*capslock*/, 0 /*F1*/, 0 /*F2*/, 0 /*F3*/,
  0 /*F4*/, 0 /*F5*/, 0 /*F6*/, 0 /*F7*/, 0 /*F8*/,
  0 /*F9*/, 0 /*F10*/, 0 /*numlock*/, 0 /*scrolllock*/, 
  0 /*home*/, 0 /*uarrow*/, 0 /*pup*/, '-' /*numminus*/,
  0 /*larrow*/, '5' /*num5*/, 0 /*rarrow*/, '+' /*numplus*/, 0 /*end*/,
  0 /*darrow*/, 0 /*pdown*/, 0 /*ins*/, 0 /*del*/, 0, 0, '|', 
  0 /*F11*/,0 /*F12*/
};

class IDT {
  
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
      this->selector = 0x08;
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
  u64int tick;
  
  void install_idt(u64int idt_ptr) {
    asm volatile("lidt (%0);"::"r"(idt_ptr):"memory");  
  }
  
  void irq_remap()
  {
    /* Initialization */
    send_byte(0x20, 0x11);
    send_byte(0xA0, 0x11);
    /* define the new vectors */
    send_byte(0x21, 0x20);
    send_byte(0xA1, 0x28);
    /* Continue initialization */
    send_byte(0x21, 0x04);
    send_byte(0xA1, 0x02);
    /* 8086/88 Mode */
    send_byte(0x21, 0x01);
    send_byte(0xA1, 0x01);
    /* Unmask all ISRs */
    send_byte(0x21, 0x00);
    send_byte(0xA1, 0x00);
  }

  u8int shift_state;
  u8int escaped;

  Monitor *monitor;
  
public:
  
    IDT(Monitor *monitor) {
      this->monitor = monitor;
      this->irq_remap();
      
      for(u64int i = 0; i < IDT_NUMBER; i++){
        this->int_gates[i].set_offsets(FIX_ADDRESS(isr_table[i]));
      }
      
      this->idtr.limit = sizeof(Interrupt_gate) * IDT_NUMBER - 1;
      this->idtr.base = FIX_ADDRESS((u64int)&this->int_gates);
      
      this->install_idt(FIX_ADDRESS((u64int) &this->idtr));
      
      this->tick = 0;
      this->shift_state = 0;
      this->escaped = 0;
    }
    
    void process_keyboard(struct context_s *s) {
      if(s->vector == 33) {
        Byte new_scan_code = get_byte(0x60);
        if (escaped) 
        {   
          //new_scan_code += 256;  // TODO: sutvarkyti "Escaped chars"
          escaped = 0;
        }
        switch(new_scan_code) {
          case 0x2a: this->shift_state = 1; break; //+LShift (2A)
          case 0x36: this->shift_state = 1; break; //+RShift (36)
          case 0xaa: this->shift_state = 0; break; //-LShift (AA)
          case 0xb6: this->shift_state = 0; break; //-RShift (B6)
          case 0xe0: this->escaped = 1; break;
          default:
            if (new_scan_code & 0x80) {
              /* Ignore the break code */
            } else {
              char new_char =(shift_state ? uppercase:lowercase)[new_scan_code];
              this->monitor->put_character(new_char);
            }
            break;
        }
        send_byte(0x20,0x20);
      }
    }
    
    void set_interrupt_gate(int number, u64int function_handler) {
      this->int_gates[number].set_offsets(function_handler);
      this->int_gates[number].set_selector(0x08);
      this->int_gates[number].set_flags(0x8E);
    }
    
    void set_interrupt_gate(int number, u64int function_handler, 
                            Byte flags) {
      this->int_gates[number].set_offsets(function_handler);
      this->int_gates[number].set_flags(flags);
      this->int_gates[number].set_selector(0x08);
    }
    
    void set_interrupt_gate(int number, u64int function_handler, 
                            Byte flags, u16int selector) {
      this->int_gates[number].set_offsets(function_handler);
      this->int_gates[number].set_flags(flags);
      this->int_gates[number].set_selector(selector);
    }
    
    void set_interrupt_gate(int number, u64int function_handler, 
                            u16int selector) {
      this->int_gates[number].set_offsets(function_handler);
      this->int_gates[number].set_flags(0x8E);
      this->int_gates[number].set_selector(selector);
    }
    
    void process_timer(struct context_s *s) {
      this->tick++;
      if (s->vector == 32){
        if(this->tick < 4){
          this->monitor->write_string("Tick: ");
          this->monitor->write_dec((u32int)this->tick);
          this->monitor->write_string("\n");
        }
      }
    }
    
    void process_interrupt(struct context_s *s)
    {
      if(s->vector < 32)
      {
        this->monitor->write_string("\nInterrupt occured: #");
        this->monitor->write_dec((u32int)s->vector);
        this->monitor->write_string(". Info: ");
        this->monitor->write_string(exception[s->vector].mnemonic);
        this->monitor->write_string(" - ");
        this->monitor->write_string(exception[s->vector].description);
        this->monitor->write_string(".\n");
        asm volatile(" cli; hlt; ");
        
      } else if(s->vector < 40) {
        this->monitor->write_string("\nIRQ occured: #");
        this->monitor->write_dec((u32int)s->vector - 32);
        this->monitor->write_string(". Info: Master IRQ.");
        /* reset Master PIC */
        send_byte(0x21, 0x20);
        
      } else if(s->vector < 48) {
        this->monitor->write_string("\nIRQ occured: #");
        this->monitor->write_dec((u32int)s->vector - 32);
        this->monitor->write_string(". Info: Slave IRQ.");
        /* reset both PICs */
        send_byte(0xA0, 0x20);
        send_byte(0x20, 0x20);
        
      } else if(s->vector < 64) {
        this->monitor->write_string("\nIRQ occured: #");
        this->monitor->write_dec((u32int)s->vector - 32);
        this->monitor->write_string(
          ". Info: non-device interrupt, caused by software.");
        
      } else {
        this->monitor->write_string("\nInterrupt occured: #");
        this->monitor->write_dec((u32int)s->vector);
        this->monitor->write_string(
          ". Info: Not implemented int number. Doing nothing.");
      }
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