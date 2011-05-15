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
#include "timer.h"
#include "keyboard.h"
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
Timer timer(&monitor);
Keyboard kbd(&monitor);

KernelPager kernel_pager(0x00000000011fa000, 0x103000);


extern "C" void default_interrupt_handler(struct context_s *s){
  if(s->vector == 32){
    timer.process_timer(s);
  } else if(s->vector == 33){
    kbd.process_keyboard(s);
  } else {
    idt.process_interrupt(s);
  }
}


extern "C" int main() {

  // Aktyvuojam savo puslapiavimą.
  kernel_pager.activate();

  // Testai.
  //test_debug();
  test_monitor(&monitor);
  //test_idt();
  enable_PIT(&pit);

  return 0xBABADEAD;
  }
