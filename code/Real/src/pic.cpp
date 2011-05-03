#include "types.h"
#include "primitives.h"
#include "pic.h"

idt_entry_t interrupt_gate[64];
idtr_t idtr;
extern "C" IntPtr isr_table[64];
isr_t interrupt_handlers[64];



static void set_interrupt_gate(const Byte id, const u64int offset, 
                               const Byte flags)
{
  interrupt_gate[id].selector = 0x18;
  interrupt_gate[id].flags = flags;
  interrupt_gate[id].zero = (Byte)(0);
  interrupt_gate[id].offset_0_15 = (u16int)(offset & 0xFFFF);
  interrupt_gate[id].offset_16_31 = (u16int)((offset >> 16) & 0xFFFF);
  
  interrupt_gate[id].offset_32_63 = (u32int)((offset >> 32) & 0xFFFFFFFF);
  interrupt_gate[id].reserved = 0;
}

// static void install_idt(const u64int idt_ptr)
// {
//   asm volatile("lidt (%0);"::"r"((u64int)idt_ptr):"memory");
// }

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

void default_interrupt_handler(struct context_s *s)
{
  if(s->vector < 32)
  {
//     monitor->write_string("Interrupt occurred: #");
//     monitor->write_string(exception[s->vector].mnemonic);
//     monitor->write_string(exception[s->vector].description);
    char symbl;
    symbl = (char)exception[s->vector].mnemonic[0];
    send_byte(0x0E9, symbl);
    symbl = (char)exception[s->vector].mnemonic[1];
    send_byte(0x0E9, symbl);
    const char * ints = "int";
    send_byte(0x0E9, (u8int)ints[0]);
    send_byte(0x0E9, (u8int)ints[1]);
    send_byte(0x0E9, (u8int)ints[2]);
    send_byte(0x0E9, 0x0A);
//     printf("#%s (%s) occurred\n", exception[s->vector].mnemonic, exception[s->vector].description);
    asm volatile(" cli; hlt; ");
  }
}

static void irq_remap(void)
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

void InitInterrupts()
{
  u64int i;
  
  irq_remap();
  
  for(i = 0; i < 64; i++)
  {
    interrupt_handlers[i] = default_interrupt_handler;
    set_interrupt_gate((Byte)i, isr_table[i], 0x8E);
  }

  
  idtr.limit = (u16int)((sizeof(struct interrupt_gate_s) * 64) - 1);
  idtr.base = (u64int)&interrupt_gate;
  install_idt((u64int)&idtr);
//   test();
//   interrupt_handlers[7] = DeviceNotAvailableExceptionHandler;
}
