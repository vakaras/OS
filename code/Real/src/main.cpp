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
#include "paging.h"
#include "debug.h"
#include "tests/test_monitor.h"
#include "tests/test_debug.h"
#include "tests/test_idt.h"


// Globalūs kintamieji.
Monitor monitor;
IDT idt(&monitor);


extern "C" void default_interrupt_handler(struct context_s *s){
  idt.process_interrupt(s);
}


extern "C" int main() {

  // Testai.
  test_debug();
  test_monitor(&monitor);
  test_idt();

  return 0xBABADEAD;
  }
