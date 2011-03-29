//
// isr.c -- High level interrupt service routines and interrupt request handlers.
//          Part of this code is modified from Bran's kernel development tutorials.
//          Rewritten for JamesM's kernel development tutorials.
//

#include "common.h"
#include "isr.h"
#include "monitor.h"

//Monitor monitor;

// Šita funkcija yra iškviečiama iš isr_handler_stub funkcijos aprašytos
// interrupt.s
//
// Kad veiktų interrupt.s reikia paduoti ne „isr_handler“, o 
// performatuotą funkcijos vardą. Jį galima gauti įvykdžius 
// „objdump -x isr.o“.
// Plačiau: http://wiki.osdev.org/C%2B%2B_to_ASM_linkage_in_GCC
void isr_handler(registers_t regs)
{

  Array2dPointer<ScreenCharacter> vm(
      (ScreenCharacter *)0xB8000, SCREEN_WIDTH, SCREEN_HEIGHT);

  vm.set(1, 30, ScreenCharacter('b'));

  u16int *video_memory = (u16int *)0xB8000;

  *(video_memory + 30) = '0' | 0x0200;
  *(video_memory + 31) = '0' | 0x0F00;
  *(video_memory + 32) = '0' | 0x0F00;
  *(video_memory + 33) = '0' | 0x0200;

  Monitor monitor;
  monitor.set_character(2, 30, ScreenCharacter('a'));
  monitor.clear();
  monitor.write_string("received interrupt: ");
  monitor.write_dec(regs.int_no);
  monitor.put_character('\n');

  }
