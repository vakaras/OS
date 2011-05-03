#include "types.h"
#include "primitives.h"
#include "pic.h"

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
  debug_string("interrupt occured: \n");
  
  if(s->vector < 32)
  {
    char symbl;
    symbl = (char)exception[s->vector].mnemonic[0];
    send_byte(0x0E9, symbl);
    symbl = (char)exception[s->vector].mnemonic[1];
    send_byte(0x0E9, symbl);
   
    asm volatile(" cli; hlt; ");
  }
}


