#include "cxx.h"
#include "icxxabi.h"
#include "types.h"
#include "primitives.h"
#include "structures/point.h"
#include "structures/array2dpointer.h"
#include "monitor_defines.h"
#include "monitor_screen_character.h"
#include "monitor.h"
#include "idt.h"
#include "pit.h"
#include "paging.h"
#include "debug.h"
#include "tests/test_monitor.h"
#include "tests/test_debug.h"
#include "tests/test_idt.h"
#include "tests/test_pit.h"


// Globalūs kintamieji.
Monitor monitor;
IDT idt(&monitor); 
PIT pit(50);


extern "C" void default_interrupt_handler(struct context_s *s){
  if(s->vector == 32){
    idt.process_timer(s);
  } else if(s->vector == 33){
    idt.process_keyboard(s);
  } else {
    idt.process_interrupt(s);
  }
}


extern "C" int main() {

  // Testai.
  test_debug();
  test_monitor(&monitor);
  //test_idt();
  //enable_PIT(&pit);

  monitor.write_string("\n");
  u64int cr3 = 0x103000;
  asm volatile("mov %0, %%cr3" : : "r"(cr3));
  //asm volatile("mov %%cr3, %0": "=r"(cr3));
  monitor.write_hex(cr3);

  u64int rsp, rbp;
  asm volatile("mov %%rsp, %0": "=r"(rsp));
  asm volatile("mov %%rbp, %0": "=r"(rbp));
  monitor.write_string("\n");
  monitor.write_hex(rsp);
  monitor.write_string("\n");
  monitor.write_hex(rbp);

  return 0xBABADEAD;
  }
